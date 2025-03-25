// SimplifyTet.h
//
// Change Simplify.h for surface meshes to SimplifyTet.h for volume meshes
//
// (C) by Damrong Guoy in 2025
//
// License : MIT
// http://opensource.org/licenses/MIT
//
// The original license for Simplify.h is below.

/////////////////////////////////////////////
//
// Mesh Simplification Tutorial
//
// (C) by Sven Forstmann in 2014
//
// License : MIT
// http://opensource.org/licenses/MIT
//
//https://github.com/sp4cerat/Fast-Quadric-Mesh-Simplification
//
// 5/2016: Chris Rorden created minimal version for OSX/Linux/Windows compile

//#include <iostream>
//#include <stddef.h>
//#include <functional>
//#include <sys/stat.h>
//#include <stdbool.h>
#include <string.h>
//#include <ctype.h>
//#include <float.h>
#include <stdio.h>
#include <stdlib.h>
#include <map>
#include <vector>
#include <string>
#include <math.h>
#include <float.h> //FLT_EPSILON, DBL_EPSILON

#include <algorithm>

#define loopi(start_l,end_l) for ( int i=start_l;i<end_l;++i )
#define loopi(start_l,end_l) for ( int i=start_l;i<end_l;++i )
#define loopj(start_l,end_l) for ( int j=start_l;j<end_l;++j )
#define loopk(start_l,end_l) for ( int k=start_l;k<end_l;++k )

struct vector3
{
double x, y, z;
};

struct vec3f
{
	double x, y, z;

	inline vec3f( void ) {}

	//inline vec3f operator =( vector3 a )
	// { vec3f b ; b.x = a.x; b.y = a.y; b.z = a.z; return b;}

	inline vec3f( vector3 a )
	 { x = a.x; y = a.y; z = a.z; }

	inline vec3f( const double X, const double Y, const double Z )
	{ x = X; y = Y; z = Z; }

	inline vec3f operator + ( const vec3f& a ) const
	{ return vec3f( x + a.x, y + a.y, z + a.z ); }

	inline vec3f operator += ( const vec3f& a ) const
	{ return vec3f( x + a.x, y + a.y, z + a.z ); }

	inline vec3f operator * ( const double a ) const
	{ return vec3f( x * a, y * a, z * a ); }

	inline vec3f operator * ( const vec3f a ) const
	{ return vec3f( x * a.x, y * a.y, z * a.z ); }

	inline vec3f v3 () const
	{ return vec3f( x , y, z ); }

	inline vec3f operator = ( const vector3 a )
	{ x=a.x;y=a.y;z=a.z;return *this; }

	inline vec3f operator = ( const vec3f a )
	{ x=a.x;y=a.y;z=a.z;return *this; }

	inline vec3f operator / ( const vec3f a ) const
	{ return vec3f( x / a.x, y / a.y, z / a.z ); }

	inline vec3f operator - ( const vec3f& a ) const
	{ return vec3f( x - a.x, y - a.y, z - a.z ); }

	inline vec3f operator / ( const double a ) const
	{ return vec3f( x / a, y / a, z / a ); }

	inline double dot( const vec3f& a ) const
	{ return a.x*x + a.y*y + a.z*z; }

	inline vec3f cross( const vec3f& a , const vec3f& b )
	{
		x = a.y * b.z - a.z * b.y;
		y = a.z * b.x - a.x * b.z;
		z = a.x * b.y - a.y * b.x;
		return *this;
	}

	inline double angle( const vec3f& v )
	{
		vec3f a = v , b = *this;
		double dot = v.x*x + v.y*y + v.z*z;
		double len = a.length() * b.length();
		if(len==0)len=0.00001f;
		double input = dot  / len;
		if (input<-1) input=-1;
		if (input>1) input=1;
		return (double) acos ( input );
	}

	inline double angle2( const vec3f& v , const vec3f& w )
	{
		vec3f a = v , b= *this;
		double dot = a.x*b.x + a.y*b.y + a.z*b.z;
		double len = a.length() * b.length();
		if(len==0)len=1;

		vec3f plane; plane.cross( b,w );

		if ( plane.x * a.x + plane.y * a.y + plane.z * a.z > 0 )
			return (double) -acos ( dot  / len );

		return (double) acos ( dot  / len );
	}

	inline vec3f rot_x( double a )
	{
		double yy = cos ( a ) * y + sin ( a ) * z;
		double zz = cos ( a ) * z - sin ( a ) * y;
		y = yy; z = zz;
		return *this;
	}
	inline vec3f rot_y( double a )
	{
		double xx = cos ( -a ) * x + sin ( -a ) * z;
		double zz = cos ( -a ) * z - sin ( -a ) * x;
		x = xx; z = zz;
		return *this;
	}
	inline void clamp( double min, double max )
	{
		if (x<min) x=min;
		if (y<min) y=min;
		if (z<min) z=min;
		if (x>max) x=max;
		if (y>max) y=max;
		if (z>max) z=max;
	}
	inline vec3f rot_z( double a )
	{
		double yy = cos ( a ) * y + sin ( a ) * x;
		double xx = cos ( a ) * x - sin ( a ) * y;
		y = yy; x = xx;
		return *this;
	}
	inline vec3f invert()
	{
		x=-x;y=-y;z=-z;return *this;
	}
	inline vec3f frac()
	{
		return vec3f(
			x-double(int(x)),
			y-double(int(y)),
			z-double(int(z))
			);
	}

	inline vec3f integer()
	{
		return vec3f(
			double(int(x)),
			double(int(y)),
			double(int(z))
			);
	}

	inline double length() const
	{
		return (double)sqrt(x*x + y*y + z*z);
	}

	inline vec3f normalize( double desired_length = 1 )
	{
		double square = sqrt(x*x + y*y + z*z);
		/*
		if (square <= 0.00001f )
		{
			x=1;y=0;z=0;
			return *this;
		}*/
		//double len = desired_length / square;
		x/=square;y/=square;z/=square;

		return *this;
	}
	static vec3f normalize( vec3f a );

	static void random_init();
	static double random_double();
	static vec3f random();

	static int random_number;

	double random_double_01(double a){
		double rnf=a*14.434252+a*364.2343+a*4213.45352+a*2341.43255+a*254341.43535+a*223454341.3523534245+23453.423412;
		int rni=((int)rnf)%100000;
		return double(rni)/(100000.0f-1.0f);
	}

	vec3f random01_fxyz(){
		x=(double)random_double_01(x);
		y=(double)random_double_01(y);
		z=(double)random_double_01(z);
		return *this;
	}

};

vec3f barycentric(const vec3f &p, const vec3f &a, const vec3f &b, const vec3f &c){
	vec3f v0 = b-a;
	vec3f v1 = c-a;
	vec3f v2 = p-a;
	double d00 = v0.dot(v0);
	double d01 = v0.dot(v1);
	double d11 = v1.dot(v1);
	double d20 = v2.dot(v0);
	double d21 = v2.dot(v1);
	double denom = d00*d11-d01*d01;
	double v = (d11 * d20 - d01 * d21) / denom;
	double w = (d00 * d21 - d01 * d20) / denom;
	double u = 1.0 - v - w;
	return vec3f(u,v,w);
}

vec3f interpolate(const vec3f &p, const vec3f &a, const vec3f &b, const vec3f &c, const vec3f attrs[3])
{
	vec3f bary = barycentric(p,a,b,c);
	vec3f out = vec3f(0,0,0);
	out = out + attrs[0] * bary.x;
	out = out + attrs[1] * bary.y;
	out = out + attrs[2] * bary.z;
	return out;
}

double min(double v1, double v2) {
	return fmin(v1,v2);
}


class SymetricMatrix {

	public:

	// Constructor

	SymetricMatrix(double c=0) { loopi(0,10) m[i] = c;  }

	SymetricMatrix(	double m11, double m12, double m13, double m14,
		            double m22, double m23, double m24,
		                        double m33, double m34,
		                                    double m44) {
		m[0] = m11;  m[1] = m12;  m[2] = m13;  m[3] = m14;
		             m[4] = m22;  m[5] = m23;  m[6] = m24;
		                          m[7] = m33;  m[8] = m34;
		                                       m[9] = m44;
	}

	// Make plane

	SymetricMatrix(double a,double b,double c,double d)
	{
		m[0] = a*a;  m[1] = a*b;  m[2] = a*c;  m[3] = a*d;
		             m[4] = b*b;  m[5] = b*c;  m[6] = b*d;
		                          m[7 ] =c*c; m[8 ] = c*d;
		                                       m[9 ] = d*d;
	}

	double operator[](int c) const { return m[c]; }

	// Determinant

	double det(	int a11, int a12, int a13,
				int a21, int a22, int a23,
				int a31, int a32, int a33) const
	{
		double det =  m[a11]*m[a22]*m[a33] + m[a13]*m[a21]*m[a32] + m[a12]*m[a23]*m[a31]
					- m[a13]*m[a22]*m[a31] - m[a11]*m[a23]*m[a32]- m[a12]*m[a21]*m[a33];
		return det;
	}

	const SymetricMatrix operator+(const SymetricMatrix& n) const
	{
		return SymetricMatrix( m[0]+n[0],   m[1]+n[1],   m[2]+n[2],   m[3]+n[3],
						                    m[4]+n[4],   m[5]+n[5],   m[6]+n[6],
						                                 m[ 7]+n[ 7], m[ 8]+n[8 ],
						                                              m[ 9]+n[9 ]);
	}

	SymetricMatrix& operator+=(const SymetricMatrix& n)
	{
		 m[0]+=n[0];   m[1]+=n[1];   m[2]+=n[2];   m[3]+=n[3];
		 m[4]+=n[4];   m[5]+=n[5];   m[6]+=n[6];   m[7]+=n[7];
		 m[8]+=n[8];   m[9]+=n[9];
		return *this;
	}

	double m[10];
};
///////////////////////////////////////////

namespace SimplifyTet
{
	// Global Variables & Strctures
	enum Attributes {
		NONE,  // unused
		NORMAL = 2,  // unused
		TEXCOORD = 4,  // used for OBJ file's texture coordinates: "vt" flag
		COLOR = 8  // unused
	};
	// v : three Vertex's of the triangle.
	// err : err[i] = error on edge i {v_i, v_i+1} of the triangle, 0 <= i < 3,
	//       err[3] = minimum error among the three edges of the triangle.
	//              = min {err[0], err[1], err[2]}.
	// deleted : 1 if this triangle has been deleted by an edge contraction,
	//           0 otherwise.
	// dirty : 0 at the beginning of each main loop (0 <= iteration < 100) in
	//         simplify_mesh().
	//         1 if the triangle was updated in update_triangles()
	// attr :  TEXCOORD if this triangle has texture coordinates uvs[3], i.e.,
	//                  the input was loaded from an OBJ file with "vt" lines,
	//                  and this triangle corresponds to the line with the
	//                  pattern "f %d/%d/%d %d/%d/%d %d/%d/%d" or
	//                  "f %d/%d %d/%d %d/%d" in the OBJ file.
	//         0 otherwise.
	// n : unit normal vector of the triangle.
	// uvs[3] : texture coordinates at the three vertices of the triangle if
	//          attr == TEXCOORD.
	// material : material index of the triangle from the OBJ file.
	//            -1 if the input mesh was loaded from an MZ3 file.
	struct Triangle { int v[3];double err[4];int deleted,dirty,attr;vec3f n;vec3f uvs[3];int material; };
	// p : position (a.k.a. coordinates) of this Vertex of the mesh.
	// tstart : index into refs, see refs[] below,
	// tcount : how many triangles share this Vertex.
	//          N.B. the {refs[k], tstart <= k < tstart + tcount} corresponds
	//          to all the triangles sharing this Vertex.
	// q : the quadric (quadratic error function) of the vertex. It's a
	//     4x4 symmetric semi-definite matrix represented as 4+3+2+1 = 10
	//     double-precision numbers.
	// border : 1 if this Vertex is on the boundary curve of the surface,
	//          0 otherwise.
	//          `border` is set in update_mesh() and used in simplify_mesh(),
	//          compact_mesh(), and calculate_error().
	struct Vertex { vec3f p;int tstart,tcount;SymetricMatrix q;int border;};
	// 0 <= tid < triangles.size(), and 0 <= tvertex < 3.
	struct Ref { int tid,tvertex; };
	std::vector<Triangle> triangles;
	std::vector<Vertex> vertices;
	// TLDR; this is a kind of run-length encoding of vertex-to-{triangles}
	// relation (one-to-many relation).
	//
	// It has the following properties:
	//
	// 1. refs.size() == 3 * triangles.size().
	//
	// 2. Each "Vertex v : vertices" has as many as v.tcount entries in refs[].
	//    They are:
	//        refs_v = { refs[i] : v.tstart <= i < v.tstart + v.tcount }.
	//    For each ref \in refs_v, ref.tid is the index of a Triangle sharing
	//    the Vertex v, i.e., the indices of the Triangle's sharing Vertex v
	//    are:
	//        { refs[i].tid : v.tstart <= i < v.tstart + v.tcount}.
	//    Furthermore, the ref.tvertex is the local index of the Vertex v in
	//    that Triangle (recall 0 <= ref.tvertex < 3), i.e.,
	//        for (v.tstart <= i < v.tstart + v.tcount) {
	//          int v_index = triangles.at(refs[i].tid).v[refs[i].tvertex];
	//          ASSERT_TRUE(&v == &vertices.at(v_index));
	//        }
	//
	// 3. Each Triangle t = triangles[t_index] corresponds to three entries
	//    in refs[]. They are { ref \in refs : ref.tid == t_index }.
	//
	// 4. Each tuple (v_index, t_index), where:
	//       - 0 <= v_index < vertices.size(),
	//       - 0 <= t_index < triangles.size(), and
	//       - v = vertices.at(v_index) is a vertex of
	//         t = triangles.at(t_index),
	//    corresponds to one entry in refs. That is the ref \in refs[] with
	//    this property:
	//      ref.tid == t_index && t.v[ref.tvertex] == v_index.
	std::vector<Ref> refs;
	// Name of mtllib in the "mtllib" line of the input OBJ file.
	std::string mtllib;
	// The names of the materials from the "usemtl" lines in the input
	// OBJ file.
	std::vector<std::string> materials;

	// Helper functions

	// Quadric error of a general point (x,y,z) with respect to the
	// quadric q of a Vertex, i.e., the "SymetricMatrix Vertex::q".
	double vertex_error(SymetricMatrix q, double x, double y, double z);

	// Return the quadric error associated with the contraction of the
	// edge between two vertices with vertex indices id_v1 and id_v2.
	// p_result = position to which the edge contraction between the two
	//            vertices should go, i.e., the position with the
	//            minimum quadric error.
	double calculate_error(int id_v1, int id_v2, vec3f &p_result);

	// Check whether any triangle sharing Vertex v0 would flip from the edge
	// contraction of v0, v1 to the position p.
	//
	// Return false if the edge contraction would be safe for triangles sharing
	// Vertex v0.
	// Return true if:
	//     - the edge contraction would flip the normal of some triangles
	//       (it would potentially create folding, wrinkle, or
	//        self-intersection in the surface) or
	//     - the edge contraction would create a needle triangle.
	//     In both cases, the caller should prohibit such a contraction.
	//
	// p  : position to which the edge contraction of v0,v1 would go.
	// i0 : global vertex index of Vertex v0; 0 <= i0 < vertices.size().
	// i1 : global vertex index of Vertex v1; 0 <= i1 < vertices.size().
	//
	// param[out] deleted : a vector of integers with deleted.size() equals
	// the number of triangles sharing Vertex v0. The entry deleted[k]
	// corresponds to the k-th triangle of Vertex v0. The deleted[k] would
	// become:
	//     - 1 if the edge contraction would delete the k-th Triangle of
	//       Vertex v0 (because that triangle also contains Vertex v1), and
	//     - no change if the edge contraction would make the k-th Triangle
	//       become a needle triangle (but the function will return true), and
	//     - 0, otherwise (that triangle will survive; or that triangle
	//       will flip its normal, but the function will return true).
	//
	// N.B. deleted[] gives additional information when this function returns
	// false (no flip, valid edge contraction).  Otherwise, it's not reliable
	// due to the early return true without going through all triangles sharing
	// Vertex v0.
	//
	// @pre deleted.size() == v0.tcount (number of triangles sharing Vertex v0).
	//
	bool flipped(vec3f p,int i0,int i1,Vertex &v0,Vertex &v1,std::vector<int> &deleted);

    // Update texture coordinates `vec3f uvs[3]` of each triangle sharing
    // Vertex v from displacing Vertex v to its new position `p`.
    //
    // i0 : unused parameter
    // v  : Vertex that will move to the new position p.
    // p  : the new position of Vertex v.
    // deleted : deleted[k] corresponds to the k-th triangle sharing Vertex v,
    //           0 <= k < v.tcount. deleted[k] is non-zero for triangle that
    //           is deleted in the edge contraction.
	void update_uvs(int i0,const Vertex &v,const vec3f &p,std::vector<int> &deleted);


	// Go through all non-deleted Triangle t (t.deleted == 0) sharing Vertex v.
	//
	// If Triangle t will be deleted due to the edge contraction, set
	// t.deleted = 1, and increment num_deleted_triangles. Do not update
	// Triangle t.{v, dirty, err}. Do not update refs[].
	//
	// @note  The to-be-deleted Triangle t will get t.deleted = 1, so next
	//        time, we won't double count the num_deleted_triangles.
    //
	// If Triangle t will survive the edge contraction,
	// - Replace Vertex v in Triangle t with Vertex vertices[i0] by setting
	//   t.v[local index of v] = i0 (this is no-op if vertices[i0] == v),
	// - Set the dirty bit t.dirty.
	// - Update the t.err[] QEM of the three edges of Triangle t.
	// - Partially update refs[] by appending refs[] with copies of
	//   corresponding entries of refs to the non-deleted Triangle t sharing
	//   Vertex v.
	//
	// @pre 0 <= i0 < vertices.size()
	//
	// i0 : global index of target Vertex, which could be either
	//      Vertex v0 that already got updated to the new position or its
	//      mate Vertex v1 in the edge contraction that will loose connection
	//      to its triangles.
	//
	// v: either Vertex v0 or Vertex v1 in the edge contraction.
	//
	// deleted: deleted[k] <=> the k-th triangle sharing Vertex v.
	//          deleted.size() == v.tcount.
	//          The k-th Triangle t of Vertex v will have its deleted flag
	//          turn on (set t.deleted = 1) when deleted[k] != 0.
	// num_deleted_triangles : accumulated counter.
	//          deleted_triangles += number of triangles sharing Vertex v
	//          that has its corresponding deleted[k] != 0.
	//
	void update_triangles(int i0,Vertex &v,std::vector<int> &deleted,
						  int &num_deleted_triangles);

	// Update the mesh.
	//
	// simplify_mesh() calls update_mesh() at initialization and also every
	// 5-th interation.
	//
	// For non-initialization, compact the triangles[] vector by dropping
	// the triangles marked for deletion (Triangle::deleted != 0).
	//
	// For any iteration, refresh {tstart, tcount} of all vertices[] and
	// {tid, tvertex} of all refs[] from the triangle-to-vertices
	// {Triangle::v[]} of triangles[].
	//
	// For the first iteration, create the {border} and the quadric matrix {q}
	// of all vertices[]. Furthermore, create the quadric error measure {err}
	// of all edges of all triangles[].
	//
	void update_mesh(int iteration);


	// Remove deleted triangles and unused vertices before writing
	// the simplified mesh.
	void compact_mesh();

	//
	// Main simplification function
	//
	// target_count  : target number of triangles
	// agressiveness : sharpness to increase the threshold.
	//                 5..8 are good numbers
	//                 more iterations yield higher quality
	//

	void simplify_mesh(int target_count, double agressiveness=7, bool verbose=false)
	{
		// init
		loopi(0,triangles.size())
		{
			triangles[i].deleted=0;
		}

		// main iteration loop
		int num_deleted_triangles=0;
		std::vector<int> deleted0,deleted1;
		const int num_triangles=triangles.size();
		for (int iteration = 0; iteration < 100; iteration ++)
		{
			if(num_triangles - num_deleted_triangles <= target_count) {
			  // We have reached the target.
			  break;
			}

			// update mesh once in a while
			// Update the mesh at initialization, and every 5th iteration
			// afterward.
			if(iteration%5==0)
			{
				update_mesh(iteration);
			}

			// clear dirty flag
			loopi(0,triangles.size()) triangles[i].dirty=0;

			// Increase the error `threshold` as we go from one iteration to
			// the next.  As a result, we will select Triangle with smaller
			// quadric error measures in the earlier iteration.
			//
			// All triangles with edges below the `threshold` will be
			// candidates for edge contractions.
			//
			// The following numbers works well for most models.
			// If it does not, try to adjust the 3 parameters
			//
			double threshold = (1e-9)*pow(double(iteration+3), agressiveness);

			if ((verbose) && (iteration%5==0)) {
				printf("iteration %d - triangles %d threshold %g\n",
					   iteration, num_triangles - num_deleted_triangles,
					   threshold);
			}

			// Loop through each Triangle t = triangles[i].
			// remove vertices & mark deleted triangles
			loopi(0,triangles.size())
			{
				Triangle &t=triangles[i];
				if(t.err[3]>threshold) {
					// Skip Triangle with its minimum error measure,
					// err[3], above the `threshold`.
					continue;
				}
				if(t.deleted) {
					// Skip already deleted Triangle.
					continue;
				}
				if(t.dirty) {
					// Skip an updated triangle. It has participated in
					// the previous edge contraction.  See update_triangles().
					continue;
				}

				// Loop through the three edges of Triangle t. Consider only
				// the j-th edge, 0 <= j < 3, with its quadric error measure
				// less than the `threshold`.
				loopj(0,3)if(t.err[j]<threshold)
				{
					// The j-th edge, 0 <= j < 3, of Triangle t has
					// vertices t.v[j] and t.v[j+1] (index modulo 3).
					// Call these two Vertexes v0 and v1 with the global
					// vertex index i0 and i1, 0 <= i0, i1 < vertices.size(),
					// respectively.
					int i0=t.v[ j     ]; Vertex &v0 = vertices[i0];
					int i1=t.v[(j+1)%3]; Vertex &v1 = vertices[i1];

					// Border check
					// Skip an edge between a border vertex and a non-border
					// vertex.  We will perform edge contraction on purely
					// interior edge (both vertices are interior) or purely
					// border edge (both vertices are border).
					//
					// (DamrongGuoy): What would happen to the thin strip with
					// v0 and v1 on the "opposite" border like this
					// picture?  Should we allow the edge contraction (v0,v1)
					// or not?  Will it pinch the strip surface and create a
					// non-manifold vertex?
					//
					//                   v0
					//    ---------------●------------------
					//                   |
					//    ---------------●------------------
					//                   v1
					//
					if(v0.border != v1.border) {
						continue;
					}

					// Compute vertex to collapse to
					vec3f p;
					calculate_error(i0,i1,p);
					// deleted0[k] <=> k-th Triangle sharing Vertex v0.
					// deleted1[k] <=> k-th Triangle sharing Vertex v1.
					deleted0.resize(v0.tcount); // normals temporarily
					deleted1.resize(v1.tcount); // normals temporarily
					// don't remove if flipped
					if( flipped(p,i0,i1,v0,v1,deleted0) ) continue;

					if( flipped(p,i1,i0,v1,v0,deleted1) ) continue;

					if ( (t.attr & TEXCOORD) == TEXCOORD  )
					{
						update_uvs(i0,v0,p,deleted0);
						update_uvs(i0,v1,p,deleted1);
					}

					// not flipped, so remove edge
					// Displace v0 to the new position p, and absorb the QEM
					// from v1.q into v0.q.
					v0.p = p;
					v0.q += v1.q;
					int tstart=refs.size();

					// Call update_triangles() to update QEM t.err[] of all
					// Triangle t sharing v0 and set t.dirty flag because
					// v0 has just moved to the new position p and absorbed
					// QEM from v1.q.
					update_triangles(i0,v0,deleted0,num_deleted_triangles);
					// Call update_triangles() to replace the corresponding
					// entry of v1 in each of v1's Triangle t by the
					// Vertex v0 (i0 is the global index of v0). Essentially
					// we replace v1 by v0, whose now position and QEM has
					// just been updated.
					update_triangles(i0,v1,deleted1,num_deleted_triangles);

					// The rest of this loop update Vertex v0.{tstart, tcount}
					// from the partial updates of refs[] in the previous two
					// calls of update_triangles(). Essentially Vertex v0 will
					// refer to, through refs[], all Triangle's previously
					// sharing v0 or sharing v1.
					int tcount=refs.size()-tstart;

					if(tcount<=v0.tcount)
					{
						// save ram
						if(tcount) {
						  // N.B. The refs[tstart,...,tstart + tcount) will
						  // be copied to refs[v0.tstart,...,v0.tstart + tcount)
						  // and will become unreferenced. There would be no
						  // Vertex u with u.tstart == tstart. Calling
						  // update_mesh() to reconstruct the refs[] from
						  // triangles[].v[] will remove the dangling records.
						  //
						  // memcpy(dest, src, count)
						  memcpy(&refs[v0.tstart],&refs[tstart],tcount*sizeof(Ref));
						}
					}
					else
						// append
						// N.B.  The older refs[{v0.tstart,..., v0.tstart +
						// v0.tcount}], before this line, will become
						// unreferenced. There would be no Vertex u with
						// u.tstart pointing to those records because, after
						// this line, v0.tstart will point to the new set of
						// refs[] entries. Calling update_mesh() to reconstruct
						// the refs[] from triangles[].v[] will remove the
						// dangling records.
						v0.tstart=tstart;

					// If 0 < tcount <= v0.tcount, the records
					// refs[v0.tstart + tcount,...,v0.tstart + v0.tcount)
				    // will become unreferenced when we update v0.tcount in
					// this line. Calling update_mesh() to reconstruct the
					// refs[] from triangles[].v[] will remove the dangling
					// records.
					v0.tcount=tcount;
					break;
				}
				// done?
				if(num_triangles-num_deleted_triangles<=target_count)break;
			}
		}
		// clean up mesh
		compact_mesh();
	} //simplify_mesh()

// The following simplify_mesh_lossless(bool) version of
// simplify_mesh(int, double, bool) uses as small a QEM error
// `threshold` as possible, i.e., DBL_EPSILON.  The simplify_mesh()
// calculates the threshold from the `double aggressiveness` parameter
// and returns when the number of triangles reaches the
// `int target_count`. This version returns when it ran out of edge
// contraction with extreme accuracy (DBL_EPSILON);/ hence, the name
// "lossless". Only practically co-planar triangles are removed.
//
// This function is not called anywhere. Its call was commented out
// in the main program Main.cpp.  For now, we will hide this function
// using the "//".
//	void simplify_mesh_lossless(bool verbose=false)
//	{
//		// init
//		loopi(0,triangles.size()) triangles[i].deleted=0;
//
//		// main iteration loop
//		int deleted_triangles=0;
//		std::vector<int> deleted0,deleted1;
//		int triangle_count=triangles.size();
//		//int iteration = 0;
//		//loop(iteration,0,100)
//		for (int iteration = 0; iteration < 9999; iteration ++)
//		{
//			// update mesh constantly
//			update_mesh(iteration);
//			// clear dirty flag
//			loopi(0,triangles.size()) triangles[i].dirty=0;
//			//
//			// All triangles with edges below the threshold will be removed
//			//
//			// The following numbers works well for most models.
//			// If it does not, try to adjust the 3 parameters
//			//
//			double threshold = DBL_EPSILON; //1.0E-3 EPS;
//			if (verbose) {
//				printf("lossless iteration %d\n", iteration);
//			}
//
//			// remove vertices & mark deleted triangles
//			loopi(0,triangles.size())
//			{
//				Triangle &t=triangles[i];
//				if(t.err[3]>threshold) continue;
//				if(t.deleted) continue;
//				if(t.dirty) continue;
//
//				loopj(0,3)if(t.err[j]<threshold)
//				{
//					int i0=t.v[ j     ]; Vertex &v0 = vertices[i0];
//					int i1=t.v[(j+1)%3]; Vertex &v1 = vertices[i1];
//
//					// Border check
//					if(v0.border != v1.border)  continue;
//
//					// Compute vertex to collapse to.
//	                   // p will be the new position of that vertex.
//					vec3f p;
//					calculate_error(i0,i1,p);
//
//	                   // deleted0[k] <=> k-th Triangle sharing Vertex v0.
//	                   // deleted1[k] <=> k-th Triangle sharing Vertex v1.
//					deleted0.resize(v0.tcount); // normals temporarily
//					deleted1.resize(v1.tcount); // normals temporarily
//
//					// don't remove if flipped
//					if( flipped(p,i0,i1,v0,v1,deleted0) ) continue;
//					if( flipped(p,i1,i0,v1,v0,deleted1) ) continue;
//
//					if ( (t.attr & TEXCOORD) == TEXCOORD )
//					{
//						update_uvs(i0,v0,p,deleted0);
//						update_uvs(i0,v1,p,deleted1);
//					}
//
//					// not flipped, so remove edge
//					v0.p=p;
//					v0.q=v1.q+v0.q;
//					int tstart=refs.size();
//
//					update_triangles(i0,v0,deleted0,deleted_triangles);
//					update_triangles(i0,v1,deleted1,deleted_triangles);
//
//					int tcount=refs.size()-tstart;
//
//					if(tcount<=v0.tcount)
//					{
//						// save ram
//						if(tcount)memcpy(&refs[v0.tstart],&refs[tstart],tcount*sizeof(Ref));
//					}
//					else
//						// append
//						v0.tstart=tstart;
//
//					v0.tcount=tcount;
//					break;
//				}
//			}
//			if(deleted_triangles<=0)break;
//			deleted_triangles=0;
//		} //for each iteration
//		// clean up mesh
//		compact_mesh();
//	} //simplify_mesh_lossless()


	// Check if a triangle flips when this edge is removed

	bool flipped(vec3f p,int i0,int i1,Vertex &v0,Vertex &v1,std::vector<int> &deleted)
	{

		// Loop over all triangles sharing Vertex v0; 0 <= k < v0.tcount.
		loopk(0,v0.tcount)
		{
			// Triangle t is the k-th triangle sharing Vertex v0.
			Triangle &t=triangles[refs[v0.tstart+k].tid];
			if(t.deleted)continue;

			// s is the local index of Vertex v0 in Triangle t.
			// invariant: 0 <= s < 3.
			int s=refs[v0.tstart+k].tvertex;
			// id1 and id2 are global vertex indices of the other two
			// vertices of Triangle t.
			// invaraint: 0 <= id1, id2 < vertices.size()
			int id1=t.v[(s+1)%3];
			int id2=t.v[(s+2)%3];

			// Triangle t has both Vertex v0 and Vertex v1, whose global
			// index is i1.  The edge contraction will delete Triangle t later.
			// Book-keeping: deleted[k] <=> Triangle t.
			if(id1==i1 || id2==i1) // delete ?
			{

				deleted[k]=1;
				continue;
			}
			// The edge-contraction would change
			// Triangle t = {Vertex v0, vertices.at(id1), vertices.at(id2)}
			// to Triangle {p, vertices.at(id1), vertices.at(id2)}.
			// Check whether displacing v0 to p would flip the normal of
			// the triangle.
			vec3f d1 = vertices[id1].p-p; d1.normalize();
			vec3f d2 = vertices[id2].p-p; d2.normalize();
			if(fabs(d1.dot(d2))>0.999) {
				// If the dot product > 0.999, the Triangle {p, vertices[id1],
				// vertices[id2]} would have a needle angle at p. We don't want
				// a needle triangle, so we will declare it's a flip.
				// No change to the deleted[].
				return true;
            }
			vec3f n;
			// We have checked that the angle at p of Triangle {p,
			// vertices[id1], vertices[id2]} is not near zero, so the cross
			// product is reliable.  We will not delete this triangle.
			n.cross(d1,d2);
			n.normalize();
			deleted[k]=0;
			if(n.dot(t.n)<0.2) {
				// Triangle {p, vertices[id1], vertices[id2]} would flip the
				// normal vector relative to the original Triangle t.
				// We will prevent this edge contraction.
				return true;
            }
		}
		return false;
	}

	// update_uvs

	void update_uvs(int i0,const Vertex &v,const vec3f &p,std::vector<int> &deleted)
	{
	    // Loop through all triangles sharing Vertex v.
		loopk(0,v.tcount)
		{
		    // Look up vertex-to-triangle relation for the k-th triangle
			// of Vertex v.
			Ref &r=refs[v.tstart+k];
			// Triangle t is the k-th triangle of Vertex v.
			Triangle &t=triangles[r.tid];
			if(t.deleted)continue;
			if(deleted[k])continue;
			vec3f p1=vertices[t.v[0]].p;
			vec3f p2=vertices[t.v[1]].p;
			vec3f p3=vertices[t.v[2]].p;
			// Update texture coordinates of Vertex v in Triangle t.
			// r.tvertex is the local index of Vertex v in Triangle t,
			// 0 <= r.tvertex < 3.
			t.uvs[r.tvertex] = interpolate(p,p1,p2,p3,t.uvs);
		}
	}

	// Update triangle connections and edge error after a edge is collapsed

	void update_triangles(const int i0,Vertex &v,std::vector<int> &deleted,int &num_deleted_triangles)
	{
		// dummy output parameter to call calculate_error(); write-only.
		vec3f p;
		// Loop through all triangles sharing Vertex v: 0 <= k < v.tcount.
		loopk(0,v.tcount)
		{
			const Ref &r=refs[v.tstart+k];
			Triangle &t=triangles[r.tid];
			if(t.deleted)continue;
			if(deleted[k])
			{
				t.deleted=1;
				num_deleted_triangles++;
				continue;
			}
			t.v[r.tvertex]=i0;
			t.dirty=1;
			t.err[0]=calculate_error(t.v[0],t.v[1],p);
			t.err[1]=calculate_error(t.v[1],t.v[2],p);
			t.err[2]=calculate_error(t.v[2],t.v[0],p);
			t.err[3]=min(t.err[0],min(t.err[1],t.err[2]));
			refs.push_back(r);
		}
	}

	// compact triangles, compute edge error and build reference list

	void update_mesh(int iteration)
	{
		if(iteration>0) // compact triangles
		{
			int dst=0;
			loopi(0,triangles.size())
			if(!triangles[i].deleted)
			{
				triangles[dst++]=triangles[i];
			}
			triangles.resize(dst);
		}
		//

		// Init Reference ID list
		loopi(0,vertices.size())
		{
			vertices[i].tstart=0;
			vertices[i].tcount=0;
		}
		loopi(0,triangles.size())
		{
			Triangle &t=triangles[i];
			loopj(0,3) vertices[t.v[j]].tcount++;
		}
		int tstart=0;
		loopi(0,vertices.size())
		{
			Vertex &v=vertices[i];
			v.tstart=tstart;
			tstart+=v.tcount;
			v.tcount=0;
		}

		// Write References
		refs.resize(triangles.size()*3);
		loopi(0,triangles.size())
		{
			Triangle &t=triangles[i];
			loopj(0,3)
			{
				Vertex &v=vertices[t.v[j]];
				refs[v.tstart+v.tcount].tid=i;
				refs[v.tstart+v.tcount].tvertex=j;
				v.tcount++;
			}
		}

		if( iteration != 0 ) {
			return;
		}

		// Init Quadrics by Plane & Edge Errors
		//
		// required at the beginning ( iteration == 0 )
		// recomputing during the simplification is not required,
		// but mostly improves the result for closed meshes
		//

		// Initialize all border flags of all vertices to 0.
		loopi(0,vertices.size())
			vertices[i].border=0;

		// Loop through all Vertex vertices[i], 0 <= i < vertices.size().
		// In each iteration, we examine the one-ring neighborhood of
		// Vertex v = vertices[i]. It consists of all triangles sharing the
		// Vertex v.
		for (int i = 0; i < vertices.size(); ++i)
		{
			const Vertex &v=vertices[i];
 			// Identify boundary : vertices[].border=0,1
			// vcount and vids are parallel.
			// Invariant: vcount.size() == vids.size().
			// vcount[i] = how many times we have seen vertices[vids[i]].
			// vids[i] = global vertex index of the i-th entry.
			std::vector<int> vcount, vids;

			// Loop through all triangles sharing Vertex v, 0 <= j < v.tcount.
			// Count how many times each neighboring vertex appears.
			// The neighboring vertices that appears only one time is a border
			// vertex.  For example, see this picture of the neighborhood
			// of Vertex v:
			//
			//       Va --- v --- Ve
			//        |    /| \   |
			//        |  /  |   \ |
			//       Vb --- Vc ---Vd
			//
			// The vertices Va and Ve are border vertices because they appear
			// only once in this list of vertices of the four triangles sharing
			// Vertex v:
			//   {v, Va, Vb}, {v, Vb, Vc}, {v, Vc, Vd}, {v, Vd, Ve}
			// = {v, *Va*, Vb, v, Vb, Vc, v, Vc, Vd, v, Vd, *Ve*}.
			// All the other non-v vertices appear twice. The vertex v itself
			// appears four times, which is the number of triangles in its
			// one-ring neighborhood.
			//
			// Notice that this counting algorithm depends on neither
			// the order of triangles nor the order of vertices within
			// each triangle.
			//
			// Notice also that, in the above example, we do not set Vertex v
			// itself as a border vertex yet.  That would happen when we
			// perform the same counting algorithm on the border vertex Va
			// or the border vertex Ve.
			//
			for (int j = 0; j < v.tcount; ++j) {
				// Triangle t is the j-th triangle sharing Vertex v.
				const Triangle& t=triangles.at(refs[v.tstart + j].tid);

				// Go through the k-th vertex of Triangle t.
				for (int k = 0; k < 3; ++k) {
					// Global vertex index of the k-th vertex of Triangle t.
					const int vertex_id = t.v[k];
					// Index into entries of vcount[] and vids[] that
					// correspond to the k-th vertex of Triangle t.
					int s = std::distance(vids.begin(),
								std::find(vids.begin(), vids.end(),
										  vertex_id));
					if(s==vcount.size()) {
						// The search failed. This is the first time that
						// we see Vertex vertices[vertex_id].  We will start
						// a new entry in vcount[] and vids[].
						vcount.push_back(1);
						vids.push_back(vertex_id);
					}
					else
						vcount[s]++;
				}
			}
			// Go through all vertices in the neighborhood of Vertex v that
			// we have seen.
			for (int j = 0; j < vcount.size(); ++j) {
			    // If the vertex vertices[vids[j]] has its counter equals 1,
				// that vertex is a border vertex.
				if(vcount[j]==1) {
					vertices[vids[j]].border=1;
				}
			}
		}
		//initialize errors
		loopi(0,vertices.size())
			vertices[i].q=SymetricMatrix(0.0);

		loopi(0,triangles.size())
		{
			Triangle &t=triangles[i];
			vec3f n,p[3];
			loopj(0,3) p[j]=vertices[t.v[j]].p;
			n.cross(p[1]-p[0],p[2]-p[0]);
			n.normalize();
			t.n=n;
			loopj(0,3) vertices[t.v[j]].q +=
				SymetricMatrix(n.x,n.y,n.z,-n.dot(p[0]));
		}
		loopi(0,triangles.size())
		{
			// Calc Edge Error
			Triangle &t=triangles[i];vec3f p;
			loopj(0,3) t.err[j]=calculate_error(t.v[j],t.v[(j+1)%3],p);
			t.err[3]=min(t.err[0],min(t.err[1],t.err[2]));
		}
	}

	// Finally compact mesh before exiting

	void compact_mesh()
	{
		int dst=0;
		loopi(0,vertices.size())
		{
			vertices[i].tcount=0;
		}
		loopi(0,triangles.size())
		if(!triangles[i].deleted)
		{
			Triangle &t=triangles[i];
			triangles[dst++]=t;
			loopj(0,3)vertices[t.v[j]].tcount=1;
		}
		triangles.resize(dst);
		dst=0;
		loopi(0,vertices.size())
		if(vertices[i].tcount)
		{
			vertices[i].tstart=dst;
			vertices[dst].p=vertices[i].p;
			dst++;
		}
		loopi(0,triangles.size())
		{
			Triangle &t=triangles[i];
			loopj(0,3)t.v[j]=vertices[t.v[j]].tstart;
		}
		vertices.resize(dst);
	}

	// Error between vertex and Quadric

	double vertex_error(SymetricMatrix q, double x, double y, double z)
	{
 		return   q[0]*x*x + 2*q[1]*x*y + 2*q[2]*x*z + 2*q[3]*x + q[4]*y*y
 		     + 2*q[5]*y*z + 2*q[6]*y + q[7]*z*z + 2*q[8]*z + q[9];
	}

	// Error for one edge

	double calculate_error(int id_v1, int id_v2, vec3f &p_result)
	{
		// compute interpolated vertex

		const SymetricMatrix q = vertices[id_v1].q + vertices[id_v2].q;
		const bool border = vertices[id_v1].border & vertices[id_v2].border;
		double error=0;
		const double det = q.det(0, 1, 2, 1, 4, 5, 2, 5, 7);
		if ( det != 0 && !border )
		{

			// q_delta is invertible
			p_result.x = -1/det*(q.det(1, 2, 3, 4, 5, 6, 5, 7 , 8));	// vx = A41/det(q_delta)
			p_result.y =  1/det*(q.det(0, 2, 3, 1, 5, 6, 2, 7 , 8));	// vy = A42/det(q_delta)
			p_result.z = -1/det*(q.det(0, 1, 3, 1, 4, 6, 2, 5,  8));	// vz = A43/det(q_delta)

			error = vertex_error(q, p_result.x, p_result.y, p_result.z);
		}
		else
		{
			// det = 0 -> try to find best result
			const vec3f& p1=vertices[id_v1].p;
			const vec3f& p2=vertices[id_v2].p;
			const vec3f p3=(p1+p2)/2;
			const double error1 = vertex_error(q, p1.x,p1.y,p1.z);
			const double error2 = vertex_error(q, p2.x,p2.y,p2.z);
			const double error3 = vertex_error(q, p3.x,p3.y,p3.z);
			error = min(error1, min(error2, error3));
			if (error1 == error) p_result=p1;
			if (error2 == error) p_result=p2;
			if (error3 == error) p_result=p3;
		}
		return error;
	}

	char *trimwhitespace(char *str)
	{
		char *end;

		// Trim leading space
		while(isspace((unsigned char)*str)) str++;

		if(*str == 0)  // All spaces?
		return str;

		// Trim trailing space
		end = str + strlen(str) - 1;
		while(end > str && isspace((unsigned char)*end)) end--;

		// Write new null terminator
		*(end+1) = 0;

		return str;
	}

	//Option : Load OBJ
	void load_obj(const char* filename, bool process_uv=false){
		vertices.clear();
		triangles.clear();
		//printf ( "Loading Objects %s ... \n",filename);
		FILE* fn;
		if(filename==NULL)		return ;
		if((char)filename[0]==0)	return ;
		if ((fn = fopen(filename, "rb")) == NULL)
		{
			printf ( "File %s not found!\n" ,filename );
			return;
		}
		char line[1000];
		memset ( line,0,1000 );
		int vertex_cnt = 0;
		int material = -1;
		std::map<std::string, int> material_map;
		std::vector<vec3f> uvs;
		std::vector<std::vector<int> > uvMap;

		while(fgets( line, 1000, fn ) != NULL)
		{
			Vertex v;
			vec3f uv;

			if (strncmp(line, "mtllib", 6) == 0)
			{
				mtllib = trimwhitespace(&line[7]);
			}
			if (strncmp(line, "usemtl", 6) == 0)
			{
				std::string usemtl = trimwhitespace(&line[7]);
				if (material_map.find(usemtl) == material_map.end())
				{
					material_map[usemtl] = materials.size();
					materials.push_back(usemtl);
				}
				material = material_map[usemtl];
			}

			if ( line[0] == 'v' && line[1] == 't' )
			{
				if ( line[2] == ' ' )
				{
					if(sscanf(line,"vt %lf %lf",
						&uv.x,&uv.y)==2)
					{
						uv.z = 0;
						uvs.push_back(uv);
					} else
					if(sscanf(line,"vt %lf %lf %lf",
						&uv.x,&uv.y,&uv.z)==3)
					{
						uvs.push_back(uv);
					}
				}
			}
			else if ( line[0] == 'v' )
			{
				if ( line[1] == ' ' )
				if(sscanf(line,"v %lf %lf %lf",
					&v.p.x,	&v.p.y,	&v.p.z)==3)
				{
					vertices.push_back(v);
				}
			}
			int integers[9];
			if ( line[0] == 'f' )
			{
				Triangle t;
				bool tri_ok = false;
				bool has_uv = false;

				if(sscanf(line,"f %d %d %d",
					&integers[0],&integers[1],&integers[2])==3)
				{
					tri_ok = true;
				}else
				if(sscanf(line,"f %d// %d// %d//",
					&integers[0],&integers[1],&integers[2])==3)
				{
					tri_ok = true;
				}else
				if(sscanf(line,"f %d//%d %d//%d %d//%d",
					&integers[0],&integers[3],
					&integers[1],&integers[4],
					&integers[2],&integers[5])==6)
				{
					tri_ok = true;
				}else
				if(sscanf(line,"f %d/%d/%d %d/%d/%d %d/%d/%d",
					&integers[0],&integers[6],&integers[3],
					&integers[1],&integers[7],&integers[4],
					&integers[2],&integers[8],&integers[5])==9)
				{
					tri_ok = true;
					has_uv = true;
				}else // Add Support for v/vt only meshes
				if (sscanf(line, "f %d/%d %d/%d %d/%d",
					&integers[0], &integers[6],
					&integers[1], &integers[7],
					&integers[2], &integers[8]) == 6)
				{
					tri_ok = true;
					has_uv = true;
				}
				else
				{
					printf("unrecognized sequence\n");
					printf("%s\n",line);
					exit(1);
				}
				if ( tri_ok )
				{
					t.v[0] = integers[0]-1-vertex_cnt;
					t.v[1] = integers[1]-1-vertex_cnt;
					t.v[2] = integers[2]-1-vertex_cnt;
					t.attr = 0;

					if ( process_uv && has_uv )
					{
						std::vector<int> indices;
						indices.push_back(integers[6]-1-vertex_cnt);
						indices.push_back(integers[7]-1-vertex_cnt);
						indices.push_back(integers[8]-1-vertex_cnt);
						uvMap.push_back(indices);
						t.attr |= TEXCOORD;
					}

					t.material = material;
					//geo.triangles.push_back ( tri );
					triangles.push_back(t);
					//state_before = state;
					//state ='f';
				}
			}
		}
		if ( process_uv && uvs.size() )
		{
			loopi(0,triangles.size())
			{
				loopj(0,3)
				triangles[i].uvs[j] = uvs[uvMap[i][j]];
			}
		}

		fclose(fn);

		//printf("load_obj: vertices = %lu, triangles = %lu, uvs = %lu\n", vertices.size(), triangles.size(), uvs.size() );
	} // load_obj()

	// Optional : Store as OBJ

	void write_obj(const char* filename)
	{
		FILE *file=fopen(filename, "w");
		int cur_material = -1;
		bool has_uv = (triangles.size() && (triangles[0].attr & TEXCOORD) == TEXCOORD);

		if (!file)
		{
			printf("write_obj: can't write data file \"%s\".\n", filename);
			exit(0);
		}
		if (!mtllib.empty())
		{
			fprintf(file, "mtllib %s\n", mtllib.c_str());
		}
		loopi(0,vertices.size())
		{
			//fprintf(file, "v %lf %lf %lf\n", vertices[i].p.x,vertices[i].p.y,vertices[i].p.z);
			fprintf(file, "v %g %g %g\n", vertices[i].p.x,vertices[i].p.y,vertices[i].p.z); //more compact: remove trailing zeros
		}
		if (has_uv)
		{
			loopi(0,triangles.size()) if(!triangles[i].deleted)
			{
				fprintf(file, "vt %g %g\n", triangles[i].uvs[0].x, triangles[i].uvs[0].y);
				fprintf(file, "vt %g %g\n", triangles[i].uvs[1].x, triangles[i].uvs[1].y);
				fprintf(file, "vt %g %g\n", triangles[i].uvs[2].x, triangles[i].uvs[2].y);
			}
		}
		int uv = 1;
		loopi(0,triangles.size()) if(!triangles[i].deleted)
		{
			if (triangles[i].material != cur_material)
			{
				cur_material = triangles[i].material;
				fprintf(file, "usemtl %s\n", materials[triangles[i].material].c_str());
			}
			if (has_uv)
			{
				fprintf(file, "f %d/%d %d/%d %d/%d\n", triangles[i].v[0]+1, uv, triangles[i].v[1]+1, uv+1, triangles[i].v[2]+1, uv+2);
				uv += 3;
			}
			else
			{
				fprintf(file, "f %d %d %d\n", triangles[i].v[0]+1, triangles[i].v[1]+1, triangles[i].v[2]+1);
			}
			//fprintf(file, "f %d// %d// %d//\n", triangles[i].v[0]+1, triangles[i].v[1]+1, triangles[i].v[2]+1); //more compact: remove trailing zeros
		}
		fclose(file);
	}

	#ifdef _MSC_VER
	#pragma pack(2)
	struct mz3hdr {
		uint16_t SIGNATURE, ATTR;
		uint32_t NFACE, NVERT, NSKIP;
	};
	#pragma pack()	
	#else
	struct __attribute__((__packed__)) mz3hdr {
		uint16_t SIGNATURE, ATTR;
		uint32_t NFACE, NVERT, NSKIP;
	};
	#endif

	void load_mz3(const char* filename) {
		vertices.clear();
		triangles.clear();
		const int kMaterial = -1;
		std::map<std::string, int> material_map;
		std::vector<vec3f> uvs;
		std::vector<std::vector<int> > uvMap;
		FILE *fp = fopen(filename,"rb");
		struct mz3hdr h;
		size_t bytes_read = fread(&h, sizeof(struct mz3hdr), 1, fp);
		if (bytes_read <= 0) {
			printf("Unable to read %s\n", filename);
			exit(EXIT_FAILURE);
		}
		uint16_t sig = 23117;
		if (sig != h.SIGNATURE) {
			fclose(fp);
			printf("Unable to read mz3 (unable to read gz compressed) %s\n", filename);
			exit(EXIT_FAILURE);
		}
		triangles.resize(h.NFACE);
		vertices.resize(h.NVERT);
		fseek(fp, (int)(sizeof(struct mz3hdr) + h.NSKIP), SEEK_SET);
		uint32_t tribytes = h.NFACE * 3 * sizeof(uint32_t);
		uint32_t *tris = (uint32_t *) malloc(tribytes);
		bytes_read = fread(tris, tribytes, 1, fp);
		if (bytes_read <= 0) {
			printf("Unable to read triangles %s\n", filename);
			exit(EXIT_FAILURE);
		}
		uint32_t vertbytes32 = h.NVERT * 3 * sizeof(float);
		float * verts32 = (float *) malloc(vertbytes32);
		bytes_read = fread(verts32, vertbytes32, 1, fp);
		if (bytes_read <= 0) {
			printf("Unable to read vertices %s\n", filename);
			exit(EXIT_FAILURE);
		}
		int j = 0;
		for (int i = 0; i < h.NVERT; i++) {
			vertices[i].p.x = verts32[j++];
			vertices[i].p.y = verts32[j++];
			vertices[i].p.z = verts32[j++];
		}
		j = 0;
		for (int i = 0; i < h.NFACE; i++) {
			triangles[i].v[0] = tris[j++];
			triangles[i].v[1] = tris[j++];
			triangles[i].v[2] = tris[j++];
			triangles[i].attr = 0;
			triangles[i].material = kMaterial;
		}
		free(tris);
		free(verts32);
		fclose(fp);
	}
	
	void write_mz3(const char* filename) {
		// https://github.com/neurolabusc/surf-ice/tree/master/mz3
		bool has_uv = (triangles.size() && (triangles[0].attr & TEXCOORD) == TEXCOORD);
		if (has_uv) {
			printf("write_mz3 can not store UVs (use .obj format).\n");
		}
		struct mz3hdr h;
		h.SIGNATURE = 0x5A4D;
		h.ATTR = 3;//isFACE +1 isVERT +2
		h.NVERT = vertices.size();
		h.NSKIP = 0;
		h.NFACE = 0;
		for (int i = 0; i < triangles.size(); i++) {
			if(!triangles[i].deleted)
				h.NFACE++;
		}
		if ((h.NFACE < 1) || (h.NVERT < 3)) {
			printf("write_mz3: no surviving triangles.\n");
			exit(1);
		}
		FILE *fp;
		fp = fopen(filename, "wb");
		if (fp == NULL) {
			printf("write_mz3: can't write data file \"%s\".\n", filename);
			exit(1);
		}
		fwrite(&h, sizeof(struct mz3hdr), 1, fp);
		uint32_t tribytes = h.NFACE * 3 * sizeof(uint32_t);
		uint32_t *tris = (uint32_t *) malloc(tribytes);
		int j = 0;
		for (int i = 0; i < triangles.size(); i++) {
			if(triangles[i].deleted)
				continue;
			tris[j++] = triangles[i].v[0];
			tris[j++] = triangles[i].v[1];
			tris[j++] = triangles[i].v[2];
		}
		fwrite(tris, tribytes, 1, fp);
		free(tris);
		uint32_t vertbytes32 = h.NVERT * 3 * sizeof(float);
		float * verts32 = (float *) malloc(vertbytes32);
		j = 0;
		for (int i = 0; i < h.NVERT; i++) {
			verts32[j++] = vertices[i].p.x;
			verts32[j++] = vertices[i].p.y;
			verts32[j++] = vertices[i].p.z;
		}
		fwrite(verts32, vertbytes32, 1, fp);
		free(verts32);
		fclose(fp);
	}
};

///////////////////////////////////////////
