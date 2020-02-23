#include <CherrySoda/Components/Graphics/GraphicsComponent.h>

using cherrysoda::GraphicsComponent;
using cherrysoda::Math;

const Math::Mat4 GraphicsComponent::GetTransformMatrix() const
{
	return Math::TranslateMat4(
		Math::ScaleMat4(
			Math_Rotate(
				Math::TranslateMat4(
					Mat4_Identity,
				RenderPosition()),
			ZRotation(), Vec3_ZUp),
		Scale()),
	-Origin());
}
