from bcc import BPF #1
from bcc.utils import printb
from ctypes import *

device = "lo" #2

b = BPF(src_file="udp_counter.c") #3

print('source compiled')

m = b['map']
for port in range(8000, 8100):
    m[c_uint64((0x7f000001 << 32) + port)] = c_uint64(0)

print('map initialized')

fn = b.load_func("udp_counter", BPF.XDP) #4

print('func loaded')

b.attach_xdp(device, fn, 0) #5

print('func attached')

try:
    b.trace_print() #6
except KeyboardInterrupt: #7
    print('done')

b.remove_xdp(device, 0) #11
print('func detached')
