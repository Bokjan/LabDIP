#include <afx.h>
#include <cstdio>
#include <vector>
#include "CLAgent.h"

CLAgent CLAgent::clAgent;
static std::vector<cl_mem> AllocatedCLMem;

static bool GetCLPlatform(cl_platform_id *platform)
{
	cl_uint num;
	*platform = 0;
	if (clGetPlatformIDs(0, nullptr, &num) != CL_SUCCESS)
		return false;
	if (num <= 0)
		return false;
	auto platforms = new cl_platform_id[num];
	clGetPlatformIDs(num, platforms, nullptr);
	*platform = platforms[0];
	delete[] platforms;
	return true;
}

static bool GetCLDevice(cl_platform_id platform, cl_device_id **devices)
{
	cl_uint num;
	if (clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 0, nullptr, &num) != CL_SUCCESS)
		return false;
	if (num <= 0)
	{
		*devices = nullptr;
		return false;
	}
	*devices = new cl_device_id[num];
	clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, num, *devices, nullptr);
	return true;
}

CLAgent::CLAgent(void)
{
	// Step 1: get platforms and choose the first one
	cl_platform_id platform;
	VERIFY(GetCLPlatform(&platform));
	// Step 2: get devices
	VERIFY(GetCLDevice(platform, &this->Devices));
	// Step 3: create context
	this->Context = clCreateContext(nullptr, 1, this->Devices, nullptr, nullptr, nullptr);
}

CLAgent::~CLAgent(void)
{
	delete[] this->Devices;
	clReleaseContext(this->Context);
}

CLAgent::Str CLAgent::ReadFile(const char * fn)
{
	Str ret = { nullptr, 0 };
	auto fp = fopen(fn, "r");
	if (fp == nullptr)
		return ret;
	fseek(fp, 0, SEEK_END);
	ret.l = ftell(fp);
	ret.s = new char[ret.l + 1];
	ret.s[ret.l] = '\0';
	fseek(fp, 0, SEEK_SET);
	fread(ret.s, 1, ret.l, fp);
	fclose(fp);
	return ret;
}

bool CLAgent::LoadKernel(const char * fn)
{
	int status;
	auto source = this->ReadFile(fn);
	if (source.s == nullptr)
		return false;
	// Create program object
	this->Program = clCreateProgramWithSource(this->Context, 1, (const char **)&source.s, &source.l, &status);
	if (status != CL_SUCCESS)
		return false;
	// Build program
	status = clBuildProgram(this->Program, 1, this->Devices, nullptr, nullptr, nullptr);
	if (status != CL_SUCCESS)
		return false;
	// Create kernel object
	this->Kernel = clCreateKernel(this->Program, "universal_kernel", &status);
	if (status != CL_SUCCESS)
		return false;
	source.Release();
	return true;
}

cl_mem CLAgent::CreateMemoryBuffer(size_t size, void * pointer)
{
	cl_mem ret = nullptr;
	cl_int status;
	ret = clCreateBuffer(this->Context, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR, size, pointer, &status);
	if (status != CL_SUCCESS)
		return nullptr;
	AllocatedCLMem.push_back(ret);
	return ret;
}

void * CLAgent::MapBuffer(cl_mem obj, size_t size)
{
	cl_int status;
	auto ret = clEnqueueMapBuffer(this->Queue, obj, CL_TRUE, CL_MAP_READ | CL_MAP_WRITE, 0, size, 0, nullptr, nullptr, &status);
	return status == CL_SUCCESS ? ret : nullptr;
}

bool CLAgent::SetKernelArg(cl_uint index, size_t size, const void * pointer)
{
	return clSetKernelArg(this->Kernel, index, size, pointer) == CL_SUCCESS;
}

bool CLAgent::RunKernel(cl_uint work_dim, const size_t * local_work_size, const size_t * global_work_size)
{
	cl_int status;
	// Create command queue
	this->Queue = clCreateCommandQueue(this->Context, this->Devices[0], 0, &status);
	if (status != CL_SUCCESS)
		return false;
	// Launch
	cl_event event;
	status = clEnqueueNDRangeKernel(this->Queue,
		this->Kernel,
		work_dim,
		nullptr, 
		global_work_size,
		local_work_size,
		0, 
		nullptr,
		&event
	);
	if (status != CL_SUCCESS)
		return false;
	clWaitForEvents(1, &event);
	clReleaseEvent(event);
	return true;
}

void CLAgent::Cleanup(void)
{
	for (auto i : AllocatedCLMem)
		clReleaseMemObject(i);
	clReleaseKernel(this->Kernel);
	clReleaseProgram(this->Program);
	clReleaseCommandQueue(this->Queue);
}
