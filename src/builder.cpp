#include "hex_quality.h"

#include "builder.h"

//#include <emscripten.h>

//#define HL_SET_PROGRESS_PHASE(X) \
//    emscripten_run_script("HexaLab.UI.set_progress_phasename( '" X "' )");
//#define HL_SET_PROGRESS_PHASE(X) \
//    EM_ASM( HexaLab.UI.set_progress_phasename( X ) )
#define HL_SET_PROGRESS_PHASE(X)


namespace HexaLab {




inline FourIndices canonicize(const FourIndices &v) {
    // find max, put it in first place
    int m0 = (v[0]>v[1])?0:1;
    int m1 = (v[2]>v[3])?2:3;
    int m = (v[m0]>v[m1])?m0:m1;
    FourIndices f( v[m] , v[(m+1)%4], v[(m+2)%4], v[(m+3)%4]);

    // canonicize triangles: repeated vertex is a -1 in v[2]
    if (f.vi[0]==f.vi[1]) { f.vi[1]=f.vi[2]; f.vi[2]=-1; }
    if (f.vi[3]==f.vi[0]) { f.vi[3]=f.vi[2]; f.vi[2]=-1; }
    if (f.vi[1]==f.vi[2] || f.vi[2]==f.vi[3]) { f.vi[2]=-1; }

    return f;
}

static void fix_if_degenerate( Mesh& mesh, Face & f ){
    FourIndices v = mesh.vertex_indices( f );
    int matches = 0;
    if (v[0]==v[1]) matches++;
    if (v[1]==v[2]) matches++;
    if (v[2]==v[3]) matches++;
    if (v[3]==v[0]) matches++;
    // hack: degenerate (segment-shaped) faces are pointing to same hexa
    if (matches>=2) {
        f.ci[1] = f.ci[0];
        f.wi[1] = f.wi[0];
        for (short w=0; w<4; w++)
            if (v[(w+2)%4]<v[(w+3)%4]) f.wi[1] = Cell::pivot_face_around_edge(f.wi[0],w);
    }
}

void Builder::add_face(Mesh& mesh, const FaceData & fa){
    Face f;
    f.ci[0] = fa.ci;
    f.wi[0] = fa.side;
    f.ci[1] = -1;
    f.wi[1] = -1;
    mesh.compute_and_store_normal(f);
    fix_if_degenerate(mesh,f);

    Index fi = mesh.faces.size();
    mesh.faces.push_back( f );
    mesh.cells[ fa.ci ].fi[ fa.side ] = fi;
}

void Builder::add_face(Mesh& mesh, const FaceData & fa, const FaceData & fb){
    Face f;
    f.ci[0] = fa.ci;
    f.wi[0] = fa.side;
    f.ci[1] = fb.ci;
    f.wi[1] = fb.side;
    mesh.compute_and_store_normal( f );

    Index fi = mesh.faces.size();
    mesh.faces.push_back( f );
    mesh.cells[ fa.ci ].fi[ fa.side ] = fi;
    mesh.cells[ fb.ci ].fi[ fb.side ] = fi;
}

void Builder::find_matches( Mesh& mesh, std::vector<FaceData> &v, const FaceData &fd ){
    int last = v.size()-1;
    for (int i=0; i<v.size(); i++) {
        if ( v[i].matches(fd)) {

            add_face(mesh, fd, v[i]);

            // remove element i
            if (i<last) std::swap( v[i] , v[last] );
            v.pop_back();
            return;
        }
    }
    // not found: add it
    v.push_back( fd );
}

// vi: array of 8 vertex indices representing the hexa.
void Builder::add_hexa ( Mesh& mesh, const Index* vi ) {
    const Index ci = mesh.cells.size();

    Cell c;
    for (int w=0; w<8; w++)  c.vi[w] = vi[w];

    mesh.cells.push_back( c );

    for (int side=0; side<6; side++) {
        FourIndices vi = canonicize( c.get_face( side ) );
        FaceData fd(vi,ci,side);

        find_matches( mesh, face_data[ vi[0] ] , fd );
    }

}

void Builder::add_vertices( Mesh& mesh, const vector<Vector3f>& verts ){

    mesh.aabb = AlignedBox3f();

    for ( const Vector3f &v: verts ) {
        mesh.verts.emplace_back ( v );
        mesh.aabb.extend ( v );
    }
}

// mesh:     empty mesh to be filled
// vertices: mesh vertices
// indices:  8*n vertex indices
void Builder::build ( Mesh& mesh, const vector<Vector3f>& vertices, const vector<Index>& indices ) {
    assert ( indices.size() % 8 == 0 );

    mesh.verts.reserve( vertices.size() );
    mesh.faces.reserve( indices.size()/8 );

    add_vertices( mesh, vertices );

    HL_SET_PROGRESS_PHASE("Building structures");

    auto t0 = sample_time();

    face_data.clear();
    face_data.resize( vertices.size() );

    HL_LOG ( "[Builder] adding hexa " );
    add_hexas(mesh, indices ,  0, 25);
    add_hexas(mesh, indices , 25, 50);
    add_hexas(mesh, indices , 50, 75);
    add_hexas(mesh, indices , 75, 100);

    HL_LOG ( "[Builder] adding skin faces\n" );
    for (auto &v : face_data ) {
        for (FaceData &fd: v) add_face( mesh, fd );
    }

    mesh.hexa_quality.resize ( mesh.cells.size() );
    HL_LOG ( "100%%\r" );

    auto dt = milli_from_sample ( t0 );

    face_data.clear();

    HL_LOG ( "[Builder] Mesh building took %dms.\n", dt );


}

void Builder::add_hexas( Mesh& mesh, const vector<Index>& indices, int from , int to ){
    size_t c = indices.size() / 8;
    size_t first_in = c*from / 100;
    size_t first_out = c*to  / 100;

    for ( size_t h = first_in; h < first_out; h++ ) {
        add_hexa ( mesh, &indices[h * 8] );
    }
    HL_LOG ( "%d%%\r", to );
}


bool Builder::validate ( Mesh& mesh ) {
    //HL_SET_PROGRESS_PHASE("Validating");
    //HL_LOG ( "[Validator] Validation took %dms.\n", dt );
    return true;
}
}
