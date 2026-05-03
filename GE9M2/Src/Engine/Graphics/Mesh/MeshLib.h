#pragma once
#include "../RenderContext.h"
#include "Mesh.h"

class MeshLibrary {
private:
	std::vector<STATIC_VERTEX> vertices;
	std::vector<unsigned int> indices;

public:
    Mesh plane;
    Mesh cube;
	Mesh skySphere;
	Mesh debugBox;

	Mesh& getMesh(std::string name) {
		if (name == "plane")		return plane;
		if (name == "cube")			return cube;
		if (name == "skySphere")	return skySphere;
		if (name == "debugBox")		return debugBox;
	}

	MeshLibrary(RenderContext& ctx) {
		generatePlane(ctx);
		generateCube(ctx);
		generateDebugBox(ctx);
		generateSkySphere(ctx, 2000, 2000, 10000);
    }

    void generatePlane(RenderContext& ctx) {
		vertices.clear();
		indices.clear();

		vertices.push_back(addVertex(Vec3(-0.5, 0, -0.5), Vec3(0, 1, 0), Vec3(1, 0, 0), 0, 0));
		vertices.push_back(addVertex(Vec3(0.5, 0, -0.5), Vec3(0, 1, 0), Vec3(1, 0, 0), 1, 0));
		vertices.push_back(addVertex(Vec3(-0.5, 0, 0.5), Vec3(0, 1, 0), Vec3(1, 0, 0), 0, 1));
		vertices.push_back(addVertex(Vec3(0.5, 0, 0.5), Vec3(0, 1, 0), Vec3(1, 0, 0), 1, 1));
		
		indices = { 0, 1, 2, 1, 3, 2 };
		plane.createStatic(
			ctx.device().dxDevice(),
			ctx.uploader(),
			vertices,
			indices
		);
    }

	// - Local Space (0, 0.5, 0)
	// - Y = [0, 1]
	// - X/Z = [-0.5, 0.5]
	void generateCube(RenderContext& ctx) {

		vertices.clear();
		indices.clear();

		Vec3 p0 = Vec3(-0.5f, 0.0f, -0.5f);
		Vec3 p1 = Vec3(0.5f, 0.0f, -0.5f);
		Vec3 p2 = Vec3(0.5f, 1.0f, -0.5f);
		Vec3 p3 = Vec3(-0.5f, 1.0f, -0.5f);
		Vec3 p4 = Vec3(-0.5f, 0.0f, 0.5f);
		Vec3 p5 = Vec3(0.5f, 0.0f, 0.5f);
		Vec3 p6 = Vec3(0.5f, 1.0f, 0.5f);
		Vec3 p7 = Vec3(-0.5f, 1.0f, 0.5f);

		// back
		Vec3 nBack(0, 0, -1);
		Vec3 tBack(1, 0, 0);

		vertices.push_back(addVertex(p0, nBack, tBack, 0, 1));
		vertices.push_back(addVertex(p1, nBack, tBack, 1, 1));
		vertices.push_back(addVertex(p2, nBack, tBack, 1, 0));
		vertices.push_back(addVertex(p3, nBack, tBack, 0, 0));
		// front
		Vec3 nFront(0, 0, 1);
		Vec3 tFront(-1, 0, 0);

		vertices.push_back(addVertex(p5, nFront, tFront, 0, 1));
		vertices.push_back(addVertex(p4, nFront, tFront, 1, 1));
		vertices.push_back(addVertex(p7, nFront, tFront, 1, 0));
		vertices.push_back(addVertex(p6, nFront, tFront, 0, 0));

		// left
		Vec3 nLeft(-1, 0, 0);
		Vec3 tLeft(0, 0, 1);

		vertices.push_back(addVertex(p4, nLeft, tLeft, 0, 1));
		vertices.push_back(addVertex(p0, nLeft, tLeft, 1, 1));
		vertices.push_back(addVertex(p3, nLeft, tLeft, 1, 0));
		vertices.push_back(addVertex(p7, nLeft, tLeft, 0, 0));

		// right
		Vec3 nRight(1, 0, 0);
		Vec3 tRight(0, 0, -1);

		vertices.push_back(addVertex(p1, nRight, tRight, 0, 1));
		vertices.push_back(addVertex(p5, nRight, tRight, 1, 1));
		vertices.push_back(addVertex(p6, nRight, tRight, 1, 0));
		vertices.push_back(addVertex(p2, nRight, tRight, 0, 0));

		// top
		Vec3 nTop(0, 1, 0);
		Vec3 tTop(1, 0, 0);

		vertices.push_back(addVertex(p3, nTop, tTop, 0, 1));
		vertices.push_back(addVertex(p2, nTop, tTop, 1, 1));
		vertices.push_back(addVertex(p6, nTop, tTop, 1, 0));
		vertices.push_back(addVertex(p7, nTop, tTop, 0, 0));

		// bottom
		Vec3 nBottom(0, -1, 0);
		Vec3 tBottom(1, 0, 0);

		vertices.push_back(addVertex(p4, nBottom, tBottom, 0, 1));
		vertices.push_back(addVertex(p5, nBottom, tBottom, 1, 1));
		vertices.push_back(addVertex(p1, nBottom, tBottom, 1, 0));
		vertices.push_back(addVertex(p0, nBottom, tBottom, 0, 0));


		// back
		indices.push_back(0); indices.push_back(1); indices.push_back(2);
		indices.push_back(0); indices.push_back(2); indices.push_back(3);
		// front
		indices.push_back(4); indices.push_back(5); indices.push_back(6);
		indices.push_back(4); indices.push_back(6); indices.push_back(7);
		// left
		indices.push_back(8); indices.push_back(9); indices.push_back(10);
		indices.push_back(8); indices.push_back(10); indices.push_back(11);
		// right
		indices.push_back(12); indices.push_back(13); indices.push_back(14);
		indices.push_back(12); indices.push_back(14); indices.push_back(15);
		// top
		indices.push_back(16); indices.push_back(17); indices.push_back(18);
		indices.push_back(16); indices.push_back(18); indices.push_back(19);
		// bottom
		indices.push_back(20); indices.push_back(21); indices.push_back(22);
		indices.push_back(20); indices.push_back(22); indices.push_back(23);

		cube.createStatic(
			ctx.device().dxDevice(),
			ctx.uploader(),
			vertices,
			indices
		);
	}

	// - Local Space (0, 0, 0)
	// - X/Y/Z = [-0.5, 0.5]
	void generateDebugBox(RenderContext& ctx) {

		vertices.clear();
		indices.clear();

		Vec3 p0 = Vec3(-0.5f, -0.5f, -0.5f);
		Vec3 p1 = Vec3(0.5f, -0.5f, -0.5f);
		Vec3 p2 = Vec3(0.5f, 0.5f, -0.5f);
		Vec3 p3 = Vec3(-0.5f, 0.5f, -0.5f);
		Vec3 p4 = Vec3(-0.5f, -0.5f, 0.5f);
		Vec3 p5 = Vec3(0.5f, -0.5f, 0.5f);
		Vec3 p6 = Vec3(0.5f, 0.5f, 0.5f);
		Vec3 p7 = Vec3(-0.5f, 0.5f, 0.5f);

		const Vec3 zeroNormal(0, 0, 0);
		const Vec3 zeroTangent(0, 0, 0);
		const float zeroUV = 0;

		vertices.push_back(addVertex(p0, zeroNormal, zeroTangent, zeroUV, zeroUV));
		vertices.push_back(addVertex(p1, zeroNormal, zeroTangent, zeroUV, zeroUV));
		vertices.push_back(addVertex(p2, zeroNormal, zeroTangent, zeroUV, zeroUV));
		vertices.push_back(addVertex(p3, zeroNormal, zeroTangent, zeroUV, zeroUV));
		vertices.push_back(addVertex(p4, zeroNormal, zeroTangent, zeroUV, zeroUV));
		vertices.push_back(addVertex(p5, zeroNormal, zeroTangent, zeroUV, zeroUV));
		vertices.push_back(addVertex(p6, zeroNormal, zeroTangent, zeroUV, zeroUV));
		vertices.push_back(addVertex(p7, zeroNormal, zeroTangent, zeroUV, zeroUV));

		// bottom
		indices.push_back(4); indices.push_back(5);
		indices.push_back(5); indices.push_back(1);
		indices.push_back(1); indices.push_back(0);
		indices.push_back(0); indices.push_back(4);

		// top
		indices.push_back(7); indices.push_back(6);
		indices.push_back(6); indices.push_back(2);
		indices.push_back(2); indices.push_back(3);
		indices.push_back(3); indices.push_back(7);

		// Connections
		indices.push_back(4); indices.push_back(7);
		indices.push_back(5); indices.push_back(6);
		indices.push_back(1); indices.push_back(2);
		indices.push_back(0); indices.push_back(3);

		debugBox.createStatic(
			ctx.device().dxDevice(),
			ctx.uploader(),
			vertices,
			indices
		);
	}

	void generateSkySphere(RenderContext& ctx, int rings, int segments, float radius) {
		vertices.clear();
		indices.clear();

		const Vec3 zeroTangent(0, 0, 0);

		for (int lat = 0; lat <= rings; lat++) {
			float theta = lat * M_PI / rings;
			float sinTheta = sinf(theta);
			float cosTheta = cosf(theta);
			for (int lon = 0; lon <= segments; lon++) {
				float phi = lon * 2.0f * M_PI / segments;
				float sinPhi = sinf(phi);
				float cosPhi = cosf(phi);
				Vec3 position(radius * sinTheta * cosPhi, radius * cosTheta, radius * sinTheta * sinPhi);
				Vec3 normal = position.normalized();
				float tu = (float)lon / segments;
				float tv = (float)lat / rings;
				vertices.push_back(addVertex(position, normal, zeroTangent, tu, tv));
			}
		}

		for (int lat = 0; lat < rings; lat++) {
			for (int lon = 0; lon < segments; lon++) {
				int current = lat * (segments + 1) + lon;
				int next = current + segments + 1;
				indices.push_back(current);
				indices.push_back(next);
				indices.push_back(current + 1);
				indices.push_back(current + 1);
				indices.push_back(next);
				indices.push_back(next + 1);
			}
		}

		skySphere.createStatic(
			ctx.device().dxDevice(),
			ctx.uploader(),
			vertices,
			indices
		);
	}

	STATIC_VERTEX addVertex(Vec3 p, Vec3 n, Vec3 t, float tu, float tv) {
		STATIC_VERTEX v;
		v.pos = p;
		v.normal = n;
		v.tangent = t;
		v.tu = tu;
		v.tv = tv;
		return v;
	}
};
