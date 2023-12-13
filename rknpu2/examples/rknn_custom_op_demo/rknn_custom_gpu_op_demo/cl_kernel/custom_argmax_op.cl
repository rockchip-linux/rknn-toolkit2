

#pragma OPENCL EXTENSION cl_amd_printf : enable


__kernel void Argmax_float(__global float* src_buf, __global float* dst_buf)
{
  int idx = get_global_id(0); 
  unsigned int srcStride = get_global_size(0);
  dst_buf[idx] = (src_buf[idx + srcStride] > src_buf[idx]) ? 1 : 0 ;
}
