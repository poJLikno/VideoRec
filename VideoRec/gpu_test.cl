__kernel void test(__global int *message)
{
    int i = get_global_id(0);

    message[i] = i;
}
