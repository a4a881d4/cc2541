import wave
import struct
import numpy.fft

class myxcorr:
	def __init__(self,OS,seq):
		self.seq = seq
		self.os = OS
		self.pos = 0
		self.buf = [ 0 for i in range(OS*len(seq))]
		self.size = OS*len(seq)
		
	def ce(self,din):
		self.buf[self.pos]=din
		p = self.pos
		r = 0.
		for s in self.seq:
			d = 1-2*s
			r = r + d * self.buf[p]
			p = ( p + self.os ) % self.size
			
		self.pos = (self.pos+1)%self.size
		return r
		
class Fir:
	def __init__(self,coef,len):
		self.coef = coef
		self.buf = [ 0. for i in range(len) ]
		self.pos = 0
		self.len = len
		
	def ce(self,d):
		self.buf[self.pos] = d
		self.pos = ( self.pos + 1 ) % self.len
		sum = 0.
		for i in range( self.len ):
			sum = sum + self.coef[i]*self.buf[self.pos-i]
		return sum
		
		
def mypower(x):
	p = 0
	for a in x:
		p = p + a*a.conjugate()
	return p.real


class fskrx:
    def __init__(self,os):
        self.os = os
        self.buf = [ complex(1,0) for i in range(os) ]
        self.pos = 0
        
    def ce(self,a):
        FD = a * self.buf[self.pos].conjugate()/(1.+a*a.conjugate())
	self.buf[self.pos] = a
	self.pos = self.pos+1
	if self.pos==self.os:
		self.pos = 0
	return FD.imag
        
fn = 'D:/works/lb/c.wav'
f = wave.open(fn,"rb")
params = f.getparams()  
nchannels, sampwidth, framerate, nframes = params[:4]

print nchannels, sampwidth, framerate, nframes


k = f.readframes(nframes)
f.close()
phase = 0

from pylab import *

f = []

for i in range(100,400):#0,(nframes/256)-1):
	pp = i*256
	c = struct.unpack_from('512h',k,pp*4)
	d = [ complex( float(c[i]), float(c[i+1]) ) for i in range(0,len(c),2) ]
	f.append(mypower(d))

peak = max(f)

for i in range(len(f)):
	if f[i]>peak/10.:
		break

pos = i+100
g = []

f = wave.open(fn,"rb")
params = f.getparams()  
nchannels, sampwidth, framerate, nframes = params[:4]

print nchannels, sampwidth, framerate, nframes


k = f.readframes(nframes)
f.close()

for i in range(pos,pos+36):#0,(nframes/256)-1):
	pp = i*256
	c = struct.unpack_from('512h',k,pp*4)
	d = [ complex( float(c[i]), float(c[i+1]) ) for i in range(0,len(c),2) ]
	for dd in d:
		g.append(dd)



j=1.

for i in range(len(g)):
	g[i] = g[i]*j
	j = j*-1.
    
gf = numpy.fft.fft(g)


cgf = gf[1152*2:1152*6]
cg = numpy.fft.ifft(cgf)


j=1.
for i in range(len(cg)):
	cg[i] = cg[i]*j
	j = j*-1.
fp = []
for i in range(256):
    fp.append(mypower(cg[i:i+4096+16*4]))
pos = fp.index(max(fp))

cg = cg[pos:pos+4096+16*4]
ret = []
aRx = fskrx(2)

for x in cg:
	ret.append(aRx.ce(x))
    
d = [ 0x4b,0x3e,0x37,0x50 ]
seq = []
for dd in d:
	for i in range(8):
		seq.append(dd&1)
		dd = dd>>1

aX = myxcorr(2,seq)
req = []
for r in ret:
	req.append(aX.ce(r))

pos = req.index(max(req))

rec = [ ret[i] for i in range(pos-32*2,len(ret),2) ]

de = []
p = 0
d = 0
for x in rec:
	d = d>>1
	if x<0.:
		d = d|0x80
	p = p + 1
	if p==8:
		p=0
		de.append(d)
		d=0
	print p," ",d," ",x
		

           