#pragma once
#include <CL/cl.h>
class CLAgent
{
private:
	static CLAgent clAgent;
	CLAgent(void);
	~CLAgent(void);

	cl_device_id*       Devices;
	cl_context          Context;
	cl_kernel           Kernel;
	cl_program          Program;
	cl_command_queue    Queue;
public:
	struct Str
	{
		char* s;
		size_t l;
		inline void Release(void)
		{
			delete[] this->s;
		}
	};
	inline static CLAgent* GetInstance(void)
	{
		return &CLAgent::clAgent;
	}
	Str ReadFile(const char *fn);
	bool LoadKernel(const char * kernel_name);
	bool LoadKernel(const char *fn, const char * kernel_name);
	cl_mem CreateMemoryBuffer(size_t size, void *pointer);
	bool ReadBuffer(cl_mem obj, size_t size, void *dst);
	bool SetKernelArg(cl_uint index, size_t size, const void *pointer);
	bool RunKernel(cl_uint work_dim, const size_t *local_work_size, const size_t *global_work_size);
	void Cleanup(void);
};
#define CLA (CLAgent::GetInstance())
#define DECLARE_CLA(x) auto x = CLA