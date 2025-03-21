// // ReSharper disable All
#include "framework.h"
// #include "Material.h"
//
// #ifdef DO_DEBUG
// Material::Material( const string & MetaData )
// : Drawer(nullptr), CBuffer(nullptr), ECB_Color(nullptr), Textures{nullptr, }, SRVs{nullptr,}
// {
// 	CBuffer = new ConstantBuffer(&ColorData, MetaData,sizeof(ThisClass::Colors));
// }
//
// Material::Material( Shader * InDrawer, const string & MetaData )
// : Drawer(InDrawer), CBuffer(nullptr), ECB_Color(nullptr), Textures{nullptr,}, SRVs{nullptr,}
// {
// 	CBuffer = new ConstantBuffer(&ColorData, MetaData,sizeof(ThisClass::Colors));
// }
//
// Material::Material( const wstring & InShaderFileName, const string & MetaData )
// : Drawer(new Shader(InShaderFileName)), CBuffer(nullptr), ECB_Color(nullptr), Textures{nullptr,}, SRVs{nullptr,}
// {
// 	CBuffer = new ConstantBuffer(&ColorData, MetaData,sizeof(ThisClass::Colors));
// }
// #else
// Material::Material()
//  : Drawer(nullptr), CBuffer(nullptr), ECB_Color(nullptr), Textures{nullptr, }, SRVs{nullptr,}
// {
// 	CBuffer = new ConstantBuffer(ShaderType::PixelShader, 0, &ColorData, "Material.ColorData",sizeof(ThisClass::Colors));
// }
//
// Material::Material(Shader * InDrawer )
//  : Drawer(InDrawer), CBuffer(nullptr), ECB_Color(nullptr), Textures{nullptr,}, SRVs{nullptr,}
// {
// 	CBuffer = new ConstantBuffer(ShaderType::PixelShader,0, &ColorData, "Material.ColorData",sizeof(ThisClass::Colors));
// }
//
// Material::Material( const wstring & InShaderFileName )
//  : Drawer(new Shader(InShaderFileName)), CBuffer(nullptr), ECB_Color(nullptr), Textures{nullptr,}, SRVs{nullptr,}
// {
// 	CBuffer = new ConstantBuffer(ShaderType::PixelShader,0, &ColorData, "Material.ColorData",sizeof(ThisClass::Colors));
// }
// #endif
//
// Material::~Material()
// {
// 	for (int i = 0 ; i < ThisClass::MaxTextureCount ; i++)
// 	{
// 		SAFE_DELETE(Textures[i]);
// 	}
// 	SAFE_DELETE(Drawer);
// }
//
// void Material::Render()
// {
// 	if (Drawer == nullptr)
// 	{
// 		return ;
// 	}
//
// 	if (ECB_Color != nullptr)
// 		ECB_Color->SetConstantBuffer(*CBuffer);
// 	if (ESRV_TextureMap != nullptr)
// 		ESRV_TextureMap->SetResourceArray((ID3D11ShaderResourceView **)&SRVs, 0, ThisClass::MaxTextureCount);
// }
//
// void Material::SetShader( const wstring & InShaderFileName )
// {
// 	assert(InShaderFileName.length() > 0);
// 	Drawer = new Shader(InShaderFileName);
// 	ECB_Color = Drawer->AsConstantBuffer("CB_Material");
// 	ESRV_TextureMap = Drawer->AsSRV("MaterialMaps");
// }
//
// void Material::SetShader( Shader * InShader )
// {
// 	Drawer = InShader;
// 	ECB_Color = Drawer->AsConstantBuffer("CB_Material");
// 	ESRV_TextureMap = Drawer->AsSRV("MaterialMaps");
// }
//
// Shader * Material::GetShader() const
// {
// 	return Drawer;
// }
//
// void Material::SetDiffuseMap( const string & InFilePath )
// {
// 	SetDiffuseMap(String::ToWString(InFilePath));
// }
//
// void Material::SetDiffuseMap( const wstring & InWFilePath )
// {
// 	SAFE_DELETE(Textures[(int)(TextureMapType::Diffuse)]);
// 	Texture * const Tex = new Texture(InWFilePath);
// 	Textures[static_cast<int>(TextureMapType::Diffuse)] = Tex;
// 	SRVs[static_cast<int>(TextureMapType::Diffuse)] = Tex->GetSRV();
// }
//
// void Material::SetSpecularMap( const string & InFilePath )
// {
// 	SetSpecularMap(String::ToWString(InFilePath));
// }
//
// void Material::SetSpecularMap( const wstring & InWFilePath )
// {
// 	SAFE_DELETE(Textures[(int)(TextureMapType::Specular)]);
// 	Texture * const Tex = new Texture(InWFilePath);
// 	Textures[static_cast<int>(TextureMapType::Specular)] = Tex;
// 	SRVs[static_cast<int>(TextureMapType::Specular)] = Tex->GetSRV();
// }
//
// void Material::SetNormalMap( const string & InFilePath )
// {
// 	SetNormalMap(String::ToWString(InFilePath));
// }
//
// void Material::SetNormalMap( const wstring & InWFilePath )
// {
// 	SAFE_DELETE(Textures[(int)(TextureMapType::Normal)]);
// 	Texture * const Tex = new Texture(InWFilePath);
// 	Textures[static_cast<int>(TextureMapType::Normal)] = Tex;
// 	SRVs[static_cast<int>(TextureMapType::Normal)] = Tex->GetSRV();
// }
//
//
