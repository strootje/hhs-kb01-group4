#include "..\..\..\Header Files\engine\scene\Entity.h"

namespace engine
{
	//---Private attributes---
	//---Public attributes---
	//---Private methods---
	
	/**
	 * The #scripts segment in the scene file is sent to this method for interpretation.
	 * Each line interpreted this way is executed and loaded directly.
	 * @param		std::string		The script which is to be parsed and executed.
	 * @return		void
	 */
	void Entity::ParseAndExecuteScript(std::string argScript)
	{
		std::vector<std::string> parts = explode(' ', argScript);
		std::string funcName = parts.at(0);

		// Makes the entity orbit at it's place around the given axis in the given direction
		if(funcName == "doOrbitAtPlace")
		{
			std::string axis	= parts.at(1);
			std::string dir		= parts.at(2);

			float speed		= 1;
			if(dir == "CCW")
				speed		= -1;
			float rotSpeed	= (float)std::atof(parts.at(3).c_str());
			float radius	= (float)std::atof(parts.at(4).c_str());

			float* rot;
			float* pos1;
			float* pos2;

			if(axis == "x")
			{
				rot		= &this->rotation.x;
				pos1	= &this->position.y;
				pos2	= &this->position.z;
			}
			else if(axis == "y")
			{
				rot		= &this->rotation.y;
				pos1	= &this->position.x;
				pos2	= &this->position.z;
			}
			else
			{
				rot		= &this->rotation.z;
				pos1	= &this->position.x;
				pos2	= &this->position.y;
			}


			(*rot)	+= (rotSpeed * speed);
			(*pos1)	= ((sin(*rot + (D3DX_PI / 2)) * radius) * speed);
			(*pos2)	= ((cos(*rot + (D3DX_PI / 2)) * radius) * speed);
		}
		// Parses a small scriptlet that allows for basic mutation on the pos/rot/scale x/y/z
		else
		{
			unsigned long posDot		= argScript.find(".");
			std::string vectorName		= argScript.substr(0, posDot);
			std::string vectorDirection	= argScript.substr((posDot + 1), 1);
			std::string amount			= argScript.substr((posDot + 2), 2);
			std::string value			= argScript.substr((posDot + 4));

			Vector3* vector;
			if(vectorName == "position")
			{
				vector = &this->position;
			}
			else if(vectorName == "rotation")
			{
				vector = &this->rotation;
			}
			else if(vectorName == "scaling")
			{
				vector = &this->scaling;
			}

			float* direction;
			if(vectorDirection == "x")
			{
				direction = &vector->x;
			}
			else if(vectorDirection == "y")
			{
				direction = &vector->y;
			}
			else if(vectorDirection == "z")
			{
				direction = &vector->z;
			}

			if(amount == "++")
			{
				(*direction)++;
			}
			else if(amount == "--")
			{
				(*direction)--;
			}
			else if(amount == "+=")
			{
				(*direction) += (float)std::atof(value.c_str());
			}
			else if(amount == "-=")
			{
				(*direction) -= (float)std::atof(value.c_str());
			}
		}
	}

	//---Public methods---

	/**
	 * Constructs the Entity object, initialising the Window and Entity collection.
	 */
	Entity::Entity()
	{
		Logger::Log("Entity: Initializing", Logger::INFO, __FILE__, __LINE__);

		this->SetScaling(Vector3(1.0f, 1.0f, 1.0f));
		this->entities	= std::map<std::string, Entity*>();
		this->scripts	= std::list<std::string>();

		Logger::Log("Entity: Finished", Logger::INFO, __FILE__, __LINE__);
	}

	/**
	 * Destructs the Entity object.
	 */
	Entity::~Entity()
	{
		Logger::Log("Entity: Disposing", Logger::INFO, __FILE__, __LINE__);
		this->CleanUp();
	}

	/**
	 * Lazy cleanup method for destructing this object.
	 * @return		void
	 */
	void Entity::CleanUp()
	{
		//it del all
		std::map<std::string, Entity*>::iterator entityIt;
		for(entityIt = this->entities.begin(); entityIt != this->entities.end(); entityIt++)
		{
			Entity* pEntity		= entityIt->second;
			delete pEntity;
		}
		this->entities.clear();
		this->scripts.clear();
	}

	/**
	 * Default implementation of the Update method for all entities
	 * @return		void
	 */
	void Entity::Update()
	{
		// Parse and exucutes our scripts
		std::list<std::string>::iterator scriptIt;
		for(scriptIt = this->scripts.begin(); scriptIt != this->scripts.end(); scriptIt++)
		{
			std::string script	= *scriptIt;
			this->ParseAndExecuteScript(script);
		}
		
		// Update our sub entities
		std::map<std::string, Entity*>::iterator entityIt;
		for(entityIt = this->entities.begin(); entityIt != this->entities.end(); entityIt++)
		{
			Entity* pEntity		= entityIt->second;
			pEntity->Update();
		}
	}

	/**
	 * Default implementation of the Draw method for all entities
	 * @param		Renderer*				The renderer to use.
	 * @return		void
	 */
	void Entity::Draw(Renderer* argPRenderer)
	{
		D3DXMatrixIdentity(&this->matWorld);

		// Scaling
		D3DXMATRIXA16 matScaling;
		D3DXMatrixScaling(&matScaling, this->scaling.x, this->scaling.y, this->scaling.z);
		D3DXMatrixMultiply(&this->matWorld, &this->matWorld, &matScaling);
		
		// Rotation X
		D3DXMATRIXA16 matRotationX;
		D3DXMatrixRotationX(&matRotationX, this->rotation.x);
		D3DXMatrixMultiply(&this->matWorld, &this->matWorld, &matRotationX);
		// Rotation Y
		D3DXMATRIXA16 matRotationY;
		D3DXMatrixRotationY(&matRotationY, this->rotation.y);
		D3DXMatrixMultiply(&this->matWorld, &this->matWorld, &matRotationY);
		// Rotation Z
		D3DXMATRIXA16 matRotationZ;
		D3DXMatrixRotationZ(&matRotationZ, this->rotation.z);
		D3DXMatrixMultiply(&this->matWorld, &this->matWorld, &matRotationZ);

		// Position
		D3DXMATRIXA16 matPosition;
		D3DXMatrixTranslation(&matPosition, this->position.x, this->position.y, this->position.z);
		D3DXMatrixMultiply(&this->matWorld, &this->matWorld, &matPosition);

		// Multiplies the entity world matrix with the renderer's world matrix (this->matWorld * renderer->matWorld)
		argPRenderer->AddToWorldMatrix(&this->matWorld);

		// Apply the matrix transformations
		argPRenderer->TransformWorldMatrix();
		argPRenderer->TransformViewMatrix();
		argPRenderer->TransformProjectionMatrix();

		// Draw our sub entities
		std::map<std::string, Entity*>::iterator entityIt;
		for(entityIt = this->entities.begin(); entityIt != this->entities.end(); entityIt++)
		{
			Entity* pEntity		= entityIt->second;
			argPRenderer->Push();
			pEntity->Draw(argPRenderer);
			argPRenderer->Pop();
		}
	}

	/**
	 * Processes any input events
	 * @param		std::string		the bind to execute
	 * @param		float			the speed
	 * @return		void
	 */
	void Entity::InputEvent(std::string argBind, float argSpeed)
	{
		// Move Forward
		if(argBind == "move_forward")
		{
			this->position.x -= (sin(this->rotation.y) * argSpeed);
			//this->position.y += (sin(this->rotation.x) * argSpeed);
			this->position.z -= (cos(this->rotation.y) * argSpeed);
		}
		// Move Backward
		else if(argBind == "move_backward")
		{
			this->position.x += (sin(this->rotation.y) * argSpeed);
			//this->position.y -= (sin(this->rotation.x) * argSpeed);
			this->position.z += (cos(this->rotation.y) * argSpeed);
		}
		// Move Forward or Backward
		else if(argBind == "move_forward_backward")
		{
			this->position.x += (sin(this->rotation.y) * argSpeed);
			//this->position.y -= (sin(this->rotation.x) * argSpeed);
			this->position.z += (cos(this->rotation.y) * argSpeed);
		}
		// Move Left
		else if(argBind == "move_left")
		{
			this->position.x -= (sin(this->rotation.y - (D3DX_PI / 2)) * argSpeed);
			//this->position.y += (sin(this->rotation.x) * argSpeed);
			this->position.z -= (cos(this->rotation.y - (D3DX_PI / 2)) * argSpeed);
		}
		// Move Right
		else if(argBind == "move_right")
		{
			this->position.x += (sin(this->rotation.y - (D3DX_PI / 2)) * argSpeed);
			//this->position.y -= (sin(this->rotation.x) * argSpeed);
			this->position.z += (cos(this->rotation.y - (D3DX_PI / 2)) * argSpeed);
		}
		// Move Left or Right
		else if(argBind == "move_left_right")
		{
			this->position.x += (sin(this->rotation.y - (D3DX_PI / 2)) * argSpeed);
			//this->position.y -= (sin(this->rotation.x) * argSpeed);
			this->position.z += (cos(this->rotation.y - (D3DX_PI / 2)) * argSpeed);
		}
		// Move Up
		else if(argBind == "move_up")
		{
			this->position.y += argSpeed;
		}
		// Move Down
		else if(argBind == "move_down")
		{
			this->position.y -= argSpeed;
		}
		// Turn Left
		else if(argBind == "turn_left")
		{
			this->rotation.y = (float)(argSpeed / 10);
		}
		// Turn Right
		else if(argBind == "turn_right")
		{
			this->rotation.y = (float)(argSpeed / 10);
		}
		// Turn Left or Right
		else if(argBind == "turn_left_right")
		{
			this->rotation.y += (float)(argSpeed / 10);
		}
		// Pan Up
		else if(argBind == "pan_up")
		{
			this->rotation.x -= (float)(argSpeed / 10);
		}
		// Pan Down
		else if(argBind == "pan_down")
		{
			this->rotation.x += (float)(argSpeed / 10);
		}
		// Pan Up or Down
		else if(argBind == "pan_up_down")
		{
			this->rotation.x -= (float)(argSpeed / 10);
		}

		// Reset
		else if(argBind == "reset")
		{
			this->position = Vector3(0.0f, 0.0f, 0.0f);
			this->rotation = Vector3(0.0f, 0.0f, 0.0f);
			this->scaling  = Vector3(1.0f, 1.0f, 1.0f);
		}
	}

	/**
	 * Setter for the position
	 * @param		Vector3					The new position for the entity
	 * @return		void
	 */
	void Entity::SetPosition(Vector3 argPosition)
	{
		this->position = argPosition;
	}

	/**
	 * Setter for the rotation
	 * @param		Vector3					The new rotation for the entity
	 * @return		void
	 */
	void Entity::SetRotation(Vector3 argRotation)
	{
		this->rotation = argRotation;
	}

	/**
	 * Setter for the scaling
	 * @param		Vector3					The new scaling for the entity
	 * @return		void
	 */
	void Entity::SetScaling(Vector3 argScaling)
	{
		this->scaling = argScaling;
	}

	/**
	 * Getter for the position
	 * @return		Vector3					The position of the entity
	 */
	Vector3 Entity::GetPosition()
	{
		return this->position;
	}
	
	/**
	 * Getter for the rotation
	 * @return		Vector3					The rotation of the entity
	 */
	Vector3 Entity::GetRotation()
	{
		return this->rotation;
	}
	
	/**
	 * Getter for the rotation
	 * @return		Vector3					The scaling of the entity
	 */
	Vector3 Entity::GetScaling()
	{
		return this->scaling;
	}

	/**
	 * Gets an entity
	 * @param		std::string			The entity name
	 * @param		Entity*				The entity
	 */
	Entity* Entity::GetEntity(std::string argEntityName)
	{
		return this->entities[argEntityName];
	}

	/**
	 * Adds a model
	 * @param		Model*					The model to add
	 * @param		std::string				The name of the model
	 * @return		void
	 */
	void Entity::AddEntity(std::string argEntityName, Entity* argPEntity)
	{
		this->entities[argEntityName] = argPEntity;
	}
	
	/**
	 * Adds a script to the entity to be executed during the update
	 * @param		std::string				The script to add
	 * @return		void
	 */
	void Entity::AddScript(std::string argScript)
	{
		this->scripts.push_back(argScript);
	}
}