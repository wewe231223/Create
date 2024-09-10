#pragma once 

class Material {
public:
	Material(std::shared_ptr<class Shader> shader);
	~Material();

	// Shader 클래스가 friend 인 이유. 
	bool operator<(const Material& rhs) const;

private:
	std::shared_ptr<class Shader> mShader;
};