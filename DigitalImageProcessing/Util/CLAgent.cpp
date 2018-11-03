#include <cstdio>
#include <map>
#include <vector>
#include "../stdafx.h"
#include "CLAgent.h"

CLAgent CLAgent::clAgent;
static std::vector<cl_mem> AllocatedCLMem;
static std::map<std::string, std::string> CLFiles = 
{
	{"Scale", "./OpenCL/scale.cl"},
	{"Fourier", "./OpenCL/fourier.cl"}
};

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
	int status;
	this->Context = clCreateContext(nullptr, 1, this->Devices, nullptr, nullptr, &status);
	VERIFY(status == CL_SUCCESS);
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
	fseek(fp, 0, SEEK_SET);
	auto readlen = fread(ret.s, 1, ret.l, fp);
	ret.s[readlen] = '\0';
	fclose(fp);
	return ret;
}

bool CLAgent::LoadKernel(const char * kernel_name)
{

	return true;
}

bool CLAgent::LoadKernel(const char * fn, const char * kernel_name)
{
	int status;
	auto source = this->ReadFile(fn);
	if (source.s == nullptr)
		return false;
	// Create program object
	this->Program = clCreateProgramWithSource(this->Context, 1, (const char **)&source.s, nullptr, &status);
	if (status != CL_SUCCESS)
		return false;
	// Build program
	status = clBuildProgram(this->Program, 1, this->Devices, nullptr, nullptr, nullptr);
	if (status != CL_SUCCESS)
		return false;
	// Create kernel object
	this->Kernel = clCreateKernel(this->Program, kernel_name, &status);
	if (status != CL_SUCCESS)
		return false;
	source.Release();
	// Create command queue
	this->Queue = clCreateCommandQueue(this->Context, this->Devices[0], 0, &status);
	if (status != CL_SUCCESS)
		return false;
	return true;
}

cl_mem CLAgent::CreateMemoryBuffer(size_t size, void * pointer)
{
	cl_mem ret = nullptr;
	cl_int status;
	ret = clCreateBuffer(this->Context, CL_MEM_READ_WRITE, size, nullptr, &status);
	if (status != CL_SUCCESS)
		return nullptr;
	status = clEnqueueWriteBuffer(this->Queue, ret, CL_TRUE, 0, size, pointer, 0, nullptr, nullptr);
	if (status != CL_SUCCESS)
		return nullptr;
	AllocatedCLMem.push_back(ret);
	return ret;
}

bool CLAgent::ReadBuffer(cl_mem obj, size_t size, void *dst)
{
	cl_int status;
	status = clEnqueueReadBuffer(this->Queue, obj, CL_TRUE, 0, size, dst, 0, nullptr, nullptr);
	return status == CL_SUCCESS;
}

bool CLAgent::SetKernelArg(cl_uint index, size_t size, const void * pointer)
{
	auto status = clSetKernelArg(this->Kernel, index, size, pointer);
	return status == CL_SUCCESS;
}

bool CLAgent::RunKernel(cl_uint work_dim, const size_t * local_work_size, const size_t * global_work_size)
{
	cl_int status;
	// Launch
	cl_event event;
	status = clEnqueueNDRangeKernel(
		this->Queue,
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
	AllocatedCLMem.clear();
	clReleaseKernel(this->Kernel);
	clReleaseProgram(this->Program);
	clReleaseCommandQueue(this->Queue);
}
