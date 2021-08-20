#pragma once
#include "common/staticMesh3D.h"

namespace static_meshes_3D {

	/**
	* Cube static mesh with given height, width and depth.
	*/
	class Cube : public StaticMesh3D
	{
	public:
		Cube(float height, float width, float depth,
			bool withPositions = true, bool withTextureCoordinates = true, bool withNormals = true);

		void render() const override;
		void renderPoints() const override;

		/**
		 * Gets cube height.
		 */
		float getHeight() const;

		/**
		 * Gets cube width.
		 */
		float getWidth() const;

		/**
		 * Gets cube depthFL.
		 */
		float getDepth() const;

	private:
		float _height; // Cylinder radius (distance from the center of cylinder to surface)
		float _width; // Number of cylinder slices
		float _depth; // Height of the cylinder

		int _numVerticesSide; // How many vertices to render side of the cylinder
		int _numVerticesTopBottom; // How many vertices to render top / bottom of the cylinder
		int _numVerticesTotal; // Just a sum of both numbers above
		bool _isInitialized; // Boolean to keep from reinitializing same data

		void initializeData() override;
	};

} // namespace static_meshes_3D
