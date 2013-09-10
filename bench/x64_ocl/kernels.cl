__kernel void send_netin(__global float * netin, __global float * weights, __global int * weightssendidx, __global int * unitsweightidx, __global int * unitsweightlength, __global float * activations)
{
size_t tid = get_global_id(0);

for (int i = 0; i < (unitsweightlength[tid] * 0.02); i++) {
         int idx2 = unitsweightidx[tid] + i;
         int idx = weightssendidx[idx2];
         netin[idx] += weights[idx2] * activations[tid];
}

}