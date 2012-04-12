/*
 * lib_cone.h
 *
 *  Created on: Sep 28, 2010
 *      Author: anders
 */

#ifndef LIB_CONE_H_
#define LIB_CONE_H_

#include "gfanlib_matrix.h"

namespace gfan{


/**
A PolyhedralCone is represented by linear inequalities and equations. The inequalities are non-strict
and stored as the rows of a matrix and the equations are stored as rows of a second matrix.

A cone can be in one of the four states:
0) Nothing has been done to remove redundancies. This is the initial state.
1) A basis for the true, implied equations space has been computed. This means that
   the implied equations have been computed. In particular the dimension of the cone is known.
2) Redundant inequalities have been computed and have been eliminated.
   This means that the true set of facets is known - one for each element in halfSpaces.
3) The inequalities and equations from 2) have been transformed into a canonical form. Besides having
   a unique representation for the cone this also allows comparisons between cones with operator<().

Since moving for one state to the next is expensive, the user of the PolyhedralCone can specify flags
at the construction of the cone informing about which things are known.

PCP_impliedEquationsKnown means that the given set of equations generate the space of implied equations.
PCP_facetsKnown means that each inequalities describe define a (different) facet of the cone.

Each cone has the additional information: multiplicity and linear forms.
The multiplicity is an integer whose default value is one. It can be set by the user.
When a cone is projected, it can happen that the multiplicity changes according to a lattice index.
The linear forms are stored in a matrix linearForms, whose width equals the dimension of the ambient space.
The idea is that a collection of cones in this way can represent a piecewise linear function (a tropical rational function).

Caching:
When new properties are computed by changing state the information is stored in the object by updating equations and inequalities.
When some other properties are computed, such as rays the result is cached in the object. Each cached property has a corresponding flag telling if a cached value has been stored.
These methods
for these properties are considered const. Caching only works for extreme rays at the moment.

Notice:
The lineality space of a cone C is C\cap(-C).
A cone is ray if its dimension is 1+the dimension of the its lineality space.

Should the user of this class know about the states?

need to think about this...
Always putting the cone in state 1 after something has changed helps a lot.
Then all operations can be performed except comparing and getting facets with
out taking the cone to a special state.


Things to change:
- Thomas wants operations where the natural description is the dual to be fast.
  One way to achieve this is as Frank suggests to have a state -1, in which only
  the generator description is known. These should be stored in the cache. If it
  is required to move to state 0, then the inequality description is computed.
  This sounds like a reasonable solution, but of course, what we are really storing is the dual.

 - Basically all data in the object should be mutable, while almost every method should be const.

 - A method should set the cone in a given state if required. The reason for this is that
   it will be difficult for the user to figure out which state is required and therefore
   will tend to call canonicalize when not needed.

 - Cache should be added for more properties.

Optimization:
- When inequalities can be represented in 32 bit some optimizations can be done.

More things to consider:
- Does it make sense to do dimension reduction when lineality space / linear span has been
  computed?


When calling generated by rays, two flags should be passed.

 */

enum PolyhedralConePreassumptions{
  PCP_none=0,
  PCP_impliedEquationsKnown=1,
  PCP_facetsKnown=2
};

class ZCone;
ZCone intersection(const ZCone &a, const ZCone &b);
class ZCone
{
  int preassumptions;
  mutable int state;
  int n;
  Integer multiplicity;
  ZMatrix linearForms;
  mutable ZMatrix inequalities;
  mutable ZMatrix equations;
  mutable ZMatrix cachedExtremeRays;
/**
 * If this bool is true it means that cachedExtremeRays contains the extreme rays as found by extremeRays().
 */
  mutable bool haveExtremeRaysBeenCached;
  void ensureStateAsMinimum(int s)const;

  bool isInStateMinimum(int s)const;
  int getState()const;
public:
   /**
    * Constructs a polyhedral cone with specified equations and ineqalities. They are read off as rows
    * of the matrices. For efficiency it is possible to specifying a PolyhedralConePreassumptions flag
    * which tells what is known about the description already.
    */
     ZCone(ZMatrix const &inequalities_, ZMatrix const &equations_, int preassumptions_=PCP_none);

     /**
      * Get the multiplicity of the cone.
      */
     Integer getMultiplicity()const;
     /**
      * Set the multiplicity of the cone.
      */
     void setMultiplicity(Integer const &m);
     /**
      * Returns the matrix of linear forms stored in the cone object.
      */
     ZMatrix getLinearForms()const;
     /**
      * Store a matrix of linear forms in the cone object.
      */
     void setLinearForms(ZMatrix const &linearForms_);

     /**
      * Get the inequalities in the description of the cone.
      */
     ZMatrix getInequalities()const;
     /**
      * Get the equations in the description of the cone.
      */
     ZMatrix getEquations()const;
     /**
      * Compute generators of the span of the cone. They are stored as rows of the returned matrix.
      */
     ZMatrix generatorsOfSpan()const;
     /**
      * Compute generators of the lineality space of the cone. They are stored as rows of the returned matrix.
      */
     ZMatrix generatorsOfLinealitySpace()const;
     /**
      * Returns true iff it is known that every inequalities in the description defines a different facets of the cone.
      */
     bool areFacetsKnown()const{return (state>=2)||(preassumptions&PCP_facetsKnown);}
     /**
      * Returns true iff it is known that the set of equations span the space of implied equations of the description.
      */
     bool areImpliedEquationsKnown()const{return (state>=1)||(preassumptions&PCP_impliedEquationsKnown);}

     /**
      * Takes the cone to a canonical form. After taking cones to canonical form, two cones are the same
      * if and only if their matrices of equations and inequalities are the same.
      */
     void canonicalize();
     /**
      * Computes and returns the facet inequalities of the cone.
      */
     ZMatrix getFacets()const;
     /**
      * After this function has been called all inequalities describe different facets of the cone.
      */
     void findFacets();
     /**
      * The set of linear forms vanishing on the cone is a subspace. This routine returns a basis
      * of this subspace as the rows of a matrix.
      */
     ZMatrix getImpliedEquations()const;
     /**
      * After this function has been called a minimal set of implied equations for the cone is known and is
      * returned when calling getEquations(). The returned equations form a basis of the space of implied
      * equations.
      */
     void findImpliedEquations();

     /**
      * Constructor for polyhedral cone with no inequalities or equations. Tthat is, the full space of some dimension.
      */
     ZCone(int ambientDimension=0);

     /**
      * Computes are relative interior point of the cone.
      */
     ZVector getRelativeInteriorPoint()const;
  /**
     Assuming that this cone C is in state at least 3 (why not 2?), this routine returns a relative interior point v(C) of C with the following properties:
     1) v is a function, that is v(C) is found deterministically
     2) for any angle preserving, lattice preserving and lineality space preserving transformation T of R^n we have that v(T(C))=T(v(C)). This makes it easy to check if two cones in the same fan are equal up to symmetry. Here preserving the lineality space L just means T(L)=L.
  */
     ZVector getUniquePoint()const;
  /**
   * Takes a list of possible extreme rays and add up those actually contained in the cone.
   */
     ZVector getUniquePointFromExtremeRays(ZMatrix const &extremeRays)const;
     /**
      * Returns the dimension of the ambient space.
      */
     int ambientDimension()const;
     /**
      * Returns the dimension of the cone.
      */
     int dimension()const;
     /**
      * Returns (ambient dimension)-(dimension).
      */
     int codimension()const;
     /**
      * Returns the dimension of the lineality space of the cone.
      */
     int dimensionOfLinealitySpace()const;
     /**
      * Returns true iff the cone is the origin.
      */
     bool isOrigin()const;
     /**
      * Returns true iff the cone is the full space.
      */
     bool isFullSpace()const;

     /**
      * Returns the intersection of cone a and b as a cone object.
      */
     friend ZCone intersection(const ZCone &a, const ZCone &b);
     /**
      * Returns the Cartesian product of the two cones a and b.
      */
     friend ZCone product(const ZCone &a, const ZCone &b);
     /**
      * Returns the positive orthant of some dimension as a polyhedral cone.
      */
     static ZCone positiveOrthant(int dimension);
     /**
      * Returns the cone which is the sum of row span of linealitySpace and
      * the non-negative span of the rows of generators.
      */
     static ZCone givenByRays(ZMatrix const &generators, ZMatrix const &linealitySpace);

     /**
      * To use the comparison operator< the cones must have been canonicalized.
      */
     friend bool operator<(ZCone const &a, ZCone const &b);
     /**
      * To use the comparison operator!= the cones must have been canonicalized.
      */
     friend bool operator!=(ZCone const &a, ZCone const &b);

     /**
      * Returns true iff the cone contains a positive vector.
      */
     bool containsPositiveVector()const;
     /**
      * Returns true iff the cone contains the specified vector v.
      */
     bool contains(ZVector const &v)const;
     /**
      * Returns true iff the cone contains all rows of the matrix l.
      */
     bool containsRowsOf(ZMatrix const &l)const;
     /**
      * Returns true iff c is contained in the cone.
      */
     bool contains(ZCone const &c)const;
     /**
      * Returns true iff the PolyhedralCone contains v in its relative interior. False otherwise. The cone must be in state at least 1.
      */
     bool containsRelatively(ZVector const &v)const;
     /*
      * Returns true iff the cone is simplicial. That is, iff the dimension of the cone equals the number of facets.
      */
     bool isSimplicial()const;

     //PolyhedralCone permuted(IntegerVector const &v)const;

     /**
      * Returns the lineality space of the cone as a polyhedral cone.
      */
     ZCone linealitySpace()const;

     /**
      * Returns the dual cone of the cone.
      */
     ZCone dualCone()const;
     /**
      * Return -C, where C is the cone.
      */
     ZCone negated()const;
     /**
      * Compute the extreme rays of the cone, and return generators of these as the rows of a matrix.
      * The returned extreme rays are represented by vectors which are orthogonal to the lineality
      * space and which are primitive primitive.
      * This makes them unique and invariant under lattice and angle preserving linear transformations
      * in the sense that a transformed cone would give the same set of extreme rays except the
      * extreme rays have been transformed.
      * If generators for the lineality space are known, they can be supplied. This can
      * speed up computations a lot.
      */
    ZMatrix extremeRays(ZMatrix const *generatorsOfLinealitySpace=0)const;
    /**
       The cone defines two lattices, namely Z^n intersected with the
       span of the cone and Z^n intersected with the lineality space of
       the cone. Clearly the second is contained in the
       first. Furthermore, the second is a saturated lattice of the
       first. The quotient is torsion-free - hence a lattice. Generators
       of this lattice as vectors in the span of the cone are computed
       by this routine. The implied equations must be known when this
       function is called - if not the routine asserts.
     */
    ZMatrix quotientLatticeBasis()const;
    /**
       For a ray (dim=linealitydim +1)
       the quotent lattice described in quotientLatticeBasis() is
       isomorphic to Z. In fact the ray intersected with Z^n modulo the
       lineality space intersected with Z^n is a semigroup generated by
       just one element. This routine computes that element as an
       integer vector in the cone. Asserts if the cone is not a ray.
       Asserts if the implied equations have not been computed.
     */
    ZVector semiGroupGeneratorOfRay()const;

    /**
       Computes the link of the face containing v in its relative
       interior.
     */
    ZCone link(ZVector const &w)const;

    /**
       Tests if f is a face of the cone.
     */
    bool hasFace(ZCone const &f)const;
  /**
   Computes the face of the cone containing v in its relative interior.
   The vector MUST be contained in the cone.
   */
    ZCone faceContaining(ZVector const &v)const;
    /**
     * Computes the projection of the cone to the first newn coordinates.
     * The ambient space of the returned cone has dimension newn.
     */
   // PolyhedralCone projection(int newn)const;
    friend std::ostream &operator<<(std::ostream &f, ZCone const &c);
};

};




#endif /* LIB_CONE_H_ */
