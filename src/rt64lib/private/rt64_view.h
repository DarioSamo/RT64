//
// RT64
//

#pragma once

#include "rt64_common.h"

#include <map>

#include "nv_helpers_dx12/TopLevelASGenerator.h"
#include "nv_helpers_dx12/ShaderBindingTableGenerator.h"

#include "rt64_dlss.h"
#include "rt64_fsr.h"
#include "rt64_xess.h"

namespace RT64 {
	class Scene;
	class Shader;
	class Inspector;
	class Instance;
	class Texture;

	class View {
	private:
		struct RenderInstance {
			Instance *instance;
			const D3D12_VERTEX_BUFFER_VIEW* vertexBufferView;
			const D3D12_INDEX_BUFFER_VIEW* indexBufferView;
			int indexCount;
			ID3D12Resource* bottomLevelAS;
			DirectX::XMMATRIX transform;
			DirectX::XMMATRIX transformPrevious;
			RT64_MATERIAL material;
			Shader *shader;
			CD3DX12_RECT scissorRect;
			CD3DX12_VIEWPORT viewport;
			UINT flags;
		};

		struct GlobalParamsBuffer {
			XMMATRIX view;
			XMMATRIX viewI;
			XMMATRIX prevViewI;
			XMMATRIX projection;
			XMMATRIX projectionI;
			XMMATRIX viewProj;
			XMMATRIX prevViewProj;
			RT64_VECTOR4 cameraU;
			RT64_VECTOR4 cameraV;
			RT64_VECTOR4 cameraW;
			RT64_VECTOR4 viewport;
			RT64_VECTOR4 resolution;
			RT64_VECTOR4 ambientBaseColor;
			RT64_VECTOR4 ambientNoGIColor;
			RT64_VECTOR4 eyeLightDiffuseColor;
			RT64_VECTOR4 eyeLightSpecularColor;
			RT64_VECTOR4 skyDiffuseMultiplier;
			RT64_VECTOR4 skyHSLModifier;
			RT64_VECTOR2 pixelJitter;
			float skyYawOffset;
			float giDiffuseStrength;
			float giSkyStrength;
			float motionBlurStrength;
			int skyPlaneTexIndex;
			unsigned int randomSeed;
			unsigned int diSamples;
			unsigned int giSamples;
			unsigned int diReproject;
			unsigned int giReproject;
			unsigned int binaryLockMask;
			unsigned int maxLights;
			unsigned int motionBlurSamples;
			unsigned int visualizationMode;
			unsigned int frameCount;
		};

		Scene *scene;
		float fovRadians;
		float nearDist;
		float farDist;
		bool perspectiveControlActive;
		bool perspectiveCanReproject;
		AccelerationStructureBuffers topLevelASBuffers;
		nv_helpers_dx12::TopLevelASGenerator topLevelASGenerator;
		AllocatedResource rasterBg;
		ID3D12DescriptorHeap *rasterBgHeap;
		ID3D12DescriptorHeap *outputBgHeap[2];
		AllocatedResource rtOutput[2];
		AllocatedResource rtViewDirection;
		AllocatedResource rtShadingPosition;
		AllocatedResource rtShadingNormal;
		AllocatedResource rtShadingSpecular;
		AllocatedResource rtDiffuse;
		AllocatedResource rtInstanceId;
		AllocatedResource rtFirstInstanceId;
		AllocatedResource rtFirstInstanceIdReadback;
		AllocatedResource rtDirectLightAccum[2];
		AllocatedResource rtFilteredDirectLight[2];
		AllocatedResource rtIndirectLightAccum[2];
		AllocatedResource rtFilteredIndirectLight[2];
		AllocatedResource rtReflection;
		AllocatedResource rtRefraction;
		AllocatedResource rtTransparent;
		AllocatedResource rtFlow;
		AllocatedResource rtReactiveMask;
		AllocatedResource rtLockMask;
		AllocatedResource rtNormal[2];
		AllocatedResource rtDepth[2];
		AllocatedResource rtHitDistAndFlow;
		AllocatedResource rtHitColor;
		AllocatedResource rtHitNormal;
		AllocatedResource rtHitSpecular;
		AllocatedResource rtHitInstanceId;
		AllocatedResource rtOutputUpscaled;

		bool rtSwap;
		int rtWidth;
		int rtHeight;
		float resolutionScale;
		int maxReflections;
		bool rtUpscaleActive;
		UpscaleMode rtUpscaleMode;
		bool rtRecreateBuffers;
		bool rtSkipReprojection;
		bool denoiserEnabled;
		UINT rtFirstInstanceIdRowWidth;
		bool rtFirstInstanceIdReadbackUpdated;
		UINT outputRtvDescriptorSize;
		ID3D12DescriptorHeap *descriptorHeap;
		UINT descriptorHeapEntryCount;
		ID3D12DescriptorHeap *samplerHeap;
		ID3D12DescriptorHeap *composeHeap;
		ID3D12DescriptorHeap *postProcessHeap;
		ID3D12DescriptorHeap *directFilterHeaps[2];
		ID3D12DescriptorHeap *indirectFilterHeaps[2];
		nv_helpers_dx12::ShaderBindingTableGenerator sbtHelper;
		AllocatedResource sbtStorage;
		UINT64 sbtStorageSize;
		AllocatedResource globalParamBufferResource;
		GlobalParamsBuffer globalParamsBufferData;
		uint32_t globalParamsBufferSize;
		AllocatedResource filterParamBufferResource;
		uint32_t filterParamBufferSize;
		AllocatedResource activeInstancesBufferTransforms;
		uint32_t activeInstancesBufferTransformsSize;
		AllocatedResource activeInstancesBufferMaterials;
		uint32_t activeInstancesBufferMaterialsSize;
		std::vector<RenderInstance> rasterBgInstances;
		std::vector<RenderInstance> rasterFgInstances;
		std::vector<RenderInstance> rtInstances;
		std::vector<Texture *> usedTextures;
		Texture *skyPlaneTexture;
		bool scissorApplied;
		bool viewportApplied;

		// Im3D
		AllocatedResource im3dVertexBuffer;
		D3D12_VERTEX_BUFFER_VIEW im3dVertexBufferView;
		unsigned int im3dVertexCount;

		// Upscalers
		DLSS *dlss;
		FSR *fsr;
		XeSS *xess;
		Upscaler::QualityMode upscalerQuality;
		float upscalerSharpness;
		bool upscalerResolutionOverride;
		bool upscalerReactiveMask;
		bool upscalerLockMask;

		void createOutputBuffers();
		void releaseOutputBuffers();
		void createInstanceTransformsBuffer();
		void updateInstanceTransformsBuffer();
		void createInstanceMaterialsBuffer();
		void updateInstanceMaterialsBuffer();
		void createTopLevelAS(const std::vector<RenderInstance> &rtInstances);
		void createShaderResourceHeap();
		void createShaderBindingTable();
		void createGlobalParamsBuffer();
		void updateGlobalParamsBuffer();
		void createFilterParamsBuffer();
		void updateFilterParamsBuffer();
	public:
		View(Scene *scene);
		virtual ~View();
		void update();
		void render(float deltaTimeMs);
		void renderInspector(Inspector *inspector);
		void setPerspective(RT64_MATRIX4 viewMatrix, float fovRadians, float nearDist, float farDist);
		void movePerspective(RT64_VECTOR3 localMovement);
		void rotatePerspective(float localYaw, float localPitch, float localRoll);
		void setPerspectiveControlActive(bool v);
		void setPerspectiveCanReproject(bool v);
		RT64_VECTOR3 getViewPosition();
		RT64_VECTOR3 getViewDirection();
		float getFOVRadians() const;
		float getNearDistance() const;
		float getFarDistance() const;
		void setDISamples(int v);
		int getDISamples() const;
		void setGISamples(int v);
		int getGISamples() const;
		void setMaxLights(int v);
		int getMaxLights() const;
		void setMotionBlurStrength(float v);
		float getMotionBlurStrength() const;
		void setMotionBlurSamples(int v);
		int getMotionBlurSamples() const;
		void setVisualizationMode(int v);
		int getVisualizationMode() const;
		void setResolutionScale(float v);
		float getResolutionScale() const;
		void setMaxReflections(int v);
		int getMaxReflections() const;
		void setDenoiserEnabled(bool v);
		bool getDenoiserEnabled() const;
		void setUpscaleMode(UpscaleMode v);
		UpscaleMode getUpscaleMode() const;
		Upscaler *getUpscaler(UpscaleMode v) const;
		void setSkyPlaneTexture(Texture *texture);
		RT64_VECTOR3 getRayDirectionAt(int x, int y);
		RT64_INSTANCE *getRaytracedInstanceAt(int x, int y);
		void resize();
		int getWidth() const;
		int getHeight() const;
		void setUpscalerQualityMode(Upscaler::QualityMode v);
		Upscaler::QualityMode getUpscalerQualityMode();
		void setUpscalerSharpness(float v);
		float getUpscalerSharpness() const;
		void setUpscalerResolutionOverride(bool v);
		bool getUpscalerResolutionOverride() const;
		void setUpscalerReactiveMask(bool v);
		bool getUpscalerReactiveMask() const;
		void setUpscalerLockMask(bool v);
		bool getUpscalerLockMask() const;
		bool getUpscalerInitialized(UpscaleMode mode) const;
		bool getUpscalerAccelerated(UpscaleMode mode) const;
	};
};