#include "Tiger.h"

namespace engine
{
	Tiger::Tiger()
	{
		this->speed = .1;
	}

	Tiger::~Tiger()
	{
		this->CleanUp();
	}

	void Tiger::CleanUp()
	{
	}
	
	void Tiger::Load(ResourceManager* argPResourceManager, Renderer* argPRenderer)
	{
		this->SetResource(argPResourceManager->LoadResource(argPRenderer, "tiger.x"));
	}

	void Tiger::Update()
	{
		
		//this->position.y += this->speed;

		this->position.y += this->speed;
		this->rotation.y += D3DX_PI * -this->speed;
		//this->scaling.x += this->speed;
		//this->scaling.y += this->speed;
		//this->scaling.z += this->speed;

		//if(this->position.x > 3 || this->position.x < -3)
		//{
		//	this->speed *= -1;
		//}
		if(this->position.y >5){
			this->position.y = -7;
		}
	}

	void Tiger::Draw(Renderer* argPRenderer)
	{
		argPRenderer->SetupMatricis();
		DirectX9Renderer* pRenderer = (DirectX9Renderer*)argPRenderer;

		// rotation speed in ms
		int rotSpeed = 1000;

		// set the orbit matrix
		D3DXMATRIXA16 matOrbit;
		//unsigned long iTime = timeGetTime() % rotSpeed;
		//float fOrbit = iTime * ( 2.0f * D3DX_PI ) / rotSpeed;
		//D3DXMatrixRotationY( &matOrbit, 0);//fOrbit );

		D3DXMatrixScaling(&matOrbit, this->scaling.x, this->scaling.y, this->scaling.z);
		pRenderer->AddToWorldMatrix(&matOrbit);

		D3DXMatrixRotationX(&matOrbit, this->rotation.x);
		pRenderer->AddToWorldMatrix(&matOrbit);
		D3DXMatrixRotationY(&matOrbit, this->rotation.y);
		pRenderer->AddToWorldMatrix(&matOrbit);
		D3DXMatrixRotationZ(&matOrbit, this->rotation.z);
		pRenderer->AddToWorldMatrix(&matOrbit);

		D3DXMATRIXA16 matTranslation;
		D3DXMatrixTranslation(&matTranslation, this->position.x, this->position.y, this->position.z);
		pRenderer->AddToWorldMatrix(&matTranslation);

		

		// Call the parent draw to make sure the image is actually drawed
		Entity::Draw(argPRenderer);
	}
}