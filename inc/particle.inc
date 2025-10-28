
CONST PARTICLE_MAX=800

DIM Integer Particle.next

DIM Integer Particle.time(PARTICLE_MAX)
DIM Integer Particle.life(PARTICLE_MAX)
DIM Float   Particle.X(PARTICLE_MAX)
DIM Float   Particle.Y(PARTICLE_MAX)
DIM Float   Particle.dX(PARTICLE_MAX)
DIM Float   Particle.dY(PARTICLE_MAX)
DIM Integer Particle.color(PARTICLE_MAX)
DIM String  Particle.cbMove(PARTICLE_MAX)
DIM String  Particle.cbDraw(PARTICLE_MAX)
DIM String  Particle.cbEnd(PARTICLE_MAX)

sub Emitter.rocket1(x!,y!,phi!,life%,cb$)
  LOCAL Float color(10)=(7,16,18,22,26,30,34,38,42,46,50)
  LOCAL Integer n

  for n=0 to 10
    if Particle.next>=PARTICLE_MAX then exit for
    _setParticleRocket(n,x!,y!,phi!,life%)
    Particle.cbEnd(Particle.next)=cb$
    inc Particle.next
  next
end sub

sub Emitter.rocket2(x!,y!,phi!,life%,cb$)
  LOCAL Float color(10)=(7,16,18,22,26,30,34,38,42,46,50)
  LOCAL Integer n

  for n=0 to 10
    if Particle.next>=PARTICLE_MAX then exit for
    _setParticleRocket(n,x!,y!,phi!,life%)
    Particle.cbEnd(Particle.next)=choice(n=0,cb$,"")
    inc Particle.next
  next
end sub

sub Emitter.shot(x!,y!)
  LOCAL Integer n,no=CINT(4+RND()*2)
  LOCAL Float dphi=2*PI/no

  for n=0 to no-1
    if Particle.next>=PARTICLE_MAX then exit for

    Particle.time(Particle.next)=0
    Particle.life(Particle.next)=15
    Particle.X(Particle.next)=x!
    Particle.Y(Particle.next)=y!
    Particle.dX(Particle.next)=0.5*sin(n*dphi)
    Particle.dY(Particle.next)=0.5*cos(n*dphi)
    Particle.cbMove(Particle.next)="_moveLinear"
    Particle.cbDraw(Particle.next)="_drawShot"
    Particle.cbEnd(Particle.next)=""
    inc Particle.next
  next
end sub

sub Emitter.fireworks(x!,y!,no%,life%,speed!,cb$)
  LOCAL Integer n
  LOCAL Float dphi=2*PI/no%

  for n=0 to no%-1
    if Particle.next>=PARTICLE_MAX then exit for

    Particle.time(Particle.next)=0
    Particle.life(Particle.next)=life%
    Particle.X(Particle.next)=x!
    Particle.Y(Particle.next)=y!
    Particle.dX(Particle.next)=speed!*sin(n*dphi)
    Particle.dY(Particle.next)=speed!*cos(n*dphi)
    Particle.cbMove(Particle.next)="_moveDecelerated"
    Particle.cbDraw(Particle.next)="_drawFireworks"
    Particle.cbEnd(Particle.next)=cb$
    inc Particle.next
  next
end sub

sub Emitter.crackers(x!,y!,no%,life%,speed!)
  LOCAL Integer n
  LOCAL Float dphi=2*PI/no%

  for n=0 to no%-1
    if Particle.next>=PARTICLE_MAX then exit for

    Particle.time(Particle.next)=0
    Particle.life(Particle.next)=life%
    Particle.X(Particle.next)=x!
    Particle.Y(Particle.next)=y!
    Particle.dX(Particle.next)=speed!*sin(n*dphi)
    Particle.dY(Particle.next)=speed!*cos(n*dphi)
    Particle.cbMove(Particle.next)="_moveDecelerated"
    Particle.cbDraw(Particle.next)="_drawCrackers"
    Particle.cbEnd(Particle.next)=""
    inc Particle.next
  next
end sub

sub Emitter.fw1(idx%)
  Emitter.fireworks(Particle.X(idx%),Particle.Y(idx%),15,15,1,"")
  PlaySample 3,22050
end sub

sub Emitter.fw2(idx%)
  Emitter.fireworks(Particle.X(idx%),Particle.Y(idx%),19,20,1,"")
  PlaySample 3,22050
end sub

sub Emitter.fw3(idx%)
  Emitter.fireworks(Particle.X(idx%),Particle.Y(idx%),18,20,1,"Emitter.fw4")
  PlaySample 3,22050
end sub

sub Emitter.fw4(idx%)
  Emitter.crackers(Particle.X(idx%),Particle.Y(idx%),4,10,0.5)
  PlaySample 4,22050
end sub

sub Particle.init()
  Particle.next=0
end sub

sub Particle.update()
  LOCAL Integer idx

  for idx=0 to Particle.next-1
    if Particle.time(idx)>=0 then call Particle.cbMove(idx),idx
    if Particle.time(idx)<Particle.life(idx) then inc Particle.time(idx)
  next

  idx=0
  do while idx<Particle.next
    if Particle.time(idx)=Particle.life(idx) then
      if Particle.cbEnd(idx)<>"" then call Particle.cbEnd(idx),idx
      _removeParticle(idx)
    endif
    inc idx
  loop
end sub

sub Particle.draw()
  LOCAL Integer idx

  for idx=0 to Particle.next-1
    if Particle.time(idx)<0 then continue for   'deferred particles
    if isVisible(Particle.X(idx),Particle.Y(idx)) then
      call Particle.cbDraw(idx),idx  'draw particle
    endif
  next
end sub

sub _moveLinear(idx%)
  inc Particle.X(idx%),cfr(Particle.dX(idx%))
  inc Particle.Y(idx%),cfr(Particle.dY(idx%))
end sub

sub _moveDecelerated(idx%)
  inc Particle.dX(idx%),-0.025*Particle.dX(idx%)
  inc Particle.dY(idx%),-0.025*Particle.dY(idx%)
  inc Particle.X(idx%),cfr(Particle.dX(idx%))
  inc Particle.Y(idx%),cfr(Particle.dY(idx%))
end sub

sub _drawShot(idx%)
  LOCAL Float color(3)=(7,5,32,48)
  LOCAL Integer c=CINT(Particle.time(idx%)/Particle.life(idx%)*3)

  pixel Particle.X(idx%),Particle.y(idx%),map(color(c))
end sub

sub _setParticleRocket(idx%,x!,y!,phi!,life%)
  LOCAL Float color(10)=(7,16,18,22,26,30,34,38,42,46,50)

  Particle.time(Particle.next)=-idx%
  Particle.life(Particle.next)=life%
  Particle.X(Particle.next)=x!
  Particle.Y(Particle.next)=y!
  Particle.dX(Particle.next)=2*sin(phi!)
  Particle.dY(Particle.next)=2*cos(phi!)
  Particle.color(Particle.next)=map(color(idx%))
  Particle.cbMove(Particle.next)="_moveDecelerated"
  Particle.cbDraw(Particle.next)="_drawRocket"
end sub

sub _drawRocket(idx%)
  pixel Particle.X(idx%),Particle.y(idx%),Particle.color(idx%)
end sub

sub _drawFireworks(idx%)
  LOCAL Float x,y,color(17)=(9,1,9,12,4,12,10,2,10,13,5,13,14,6,14,11,3,11)
  LOCAL Integer c=CINT(Particle.time(idx%)/Particle.life(idx%)*17)

  x=Particle.X(idx%) : y=Particle.Y(idx%)
  pixel x,y-1,map(color(c))
  pixel x,y,map(color(c))
  pixel x+1,y-1,map(color(c))
  pixel x+1,y,map(color(c))
end sub

sub _drawCrackers(idx%)
  pixel Particle.X(idx%),Particle.Y(idx%),map(RND()*7)
end sub

sub _removeParticle(idx%)
  _copyParticle Particle.next-1,idx%
  inc Particle.next,-1
end sub

sub _copyParticle(idxFrom%,idxTo%)
  if idxFrom%=idxTo% then exit sub

  Particle.time(idxTo%)=Particle.time(idxFrom%)
  Particle.life(idxTo%)=Particle.life(idxFrom%)
  Particle.X(idxTo%)=Particle.X(idxFrom%)
  Particle.Y(idxTo%)=Particle.Y(idxFrom%)
  Particle.dX(idxTo%)=Particle.dX(idxFrom%)
  Particle.dY(idxTo%)=Particle.dY(idxFrom%)
  Particle.color(idxTo%)=Particle.color(idxFrom%)
  Particle.cbMove(idxTo%)=Particle.cbMove(idxFrom%)
  Particle.cbDraw(idxTo%)=Particle.cbDraw(idxFrom%)
  Particle.cbEnd(idxTo%)=Particle.cbEnd(idxFrom%)
end sub




