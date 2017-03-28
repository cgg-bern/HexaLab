#ifndef _HL_BUILDER_H
#define _HL_BUILDER_H_

#include "common.h"
#include "mesh.h"
#include "loader.h"
#include <tuple>

namespace HexaLab {
    class Builder {
    public:   
        using IndexPair = std::tuple<Index, Index>;
        using IndexQuad = std::tuple<Index, Index, Index, Index>;

    private:
        struct EdgeRef {
            Index idx;

            EdgeRef(Index idx) {
                this->idx = idx;
            }
        };

        struct FaceRef {
            Index idx;
            Index hexas[2] = { -1, -1};

            FaceRef(Index idx) {
                this->idx = idx;
            }
        };

        struct Face {
            Index hexa;
            Index verts[4];

            enum Hexa {
                Front = 0,
                Back,
            };

            enum Dart {
                BotRight = 0,
                BotLeft,
                LeftBot,
                LeftTop,
                TopLeft,
                TopRight,
                RightTop,
                RightBot,
            };
        };

        struct Hexa {
            enum Face {
                Left = 0,
                Bottom,
                Near,
                Right,
                Top,
                Far,
            };

            enum Vert {
                NearBotRight = 0,
                NearBotLeft,
                NearTopLeft,
                NearTopRight,
                FarBotRight,
                FarBotLeft,
                FarTopLeft,
                FarTopRight,
            };
        };

        static std::unordered_map<IndexPair, EdgeRef> edges_map;
        static std::unordered_map<IndexQuad, FaceRef> faces_map;

        static Mesh* s_mesh;
        static MeshData* s_data;

        static Hexa::Face opposite(Hexa::Face face);
        static void add_edge(Index h, Index f, IndexPair indices);
        static void add_face(Index h, Hexa::Face face_enum, IndexQuad indices, Face::Hexa hexa_enum);
        static void add_hexa(MeshData::Hexa hexa);

    public:
        static void build(Mesh& mesh, MeshData& data);
    };
}

namespace std {
	template <> struct hash<HexaLab::Builder::IndexPair> {
		size_t operator()(const HexaLab::Builder::IndexPair& e)const  {
			return std::get<0>(e) + std::get<1>(e);
		}
	};

    bool operator==(HexaLab::Builder::IndexPair& lhs, HexaLab::Builder::IndexPair& rhs) {
        return (std::get<0>(lhs) == std::get<0>(rhs) && std::get<1>(lhs) == std::get<1>(rhs))
            || (std::get<0>(lhs) == std::get<1>(rhs) && std::get<1>(lhs) == std::get<0>(rhs));
    }
}

namespace std {
	template <> struct hash<HexaLab::Builder::IndexQuad> {
		size_t operator()(const HexaLab::Builder::IndexQuad& f)const  {
			return std::get<0>(f) + std::get<1>(f) + std::get<2>(f) + std::get<3>(f);
		}
	};
}
    
#endif