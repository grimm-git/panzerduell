' Explosion code

CONST EXPLOSIONS_MAX=5
CONST EXPLOSIONS_SPEED=30

DIM Integer Explosion.W=50
DIM Integer Explosion.H=49
DIM Integer Explosion.frame(EXPLOSIONS_MAX)
DIM Float   Explosion.X(EXPLOSIONS_MAX)
DIM Float   Explosion.Y(EXPLOSIONS_MAX)

sub Explosion.init()
  LOCAL Integer t
  
  for t=0 to EXPLOSIONS_MAX-1
    Explosion.frame(t)=0
    Explosion.X(t)=0
    Explosion.Y(t)=0
  next
end sub

'x!,y! in playfield coordinates
sub Explosion.add(x!,y!,ch%)
  LOCAL Integer t

  for t=0 to EXPLOSIONS_MAX-1
    if Explosion.frame(t)=0 then
      Explosion.frame(t)=1
      Explosion.X(t)=x!-Explosion.W/2
      Explosion.Y(t)=y!-Explosion.H/2
      PlaySample 11,22050,ch%
      exit sub
    endif
  next
end sub

sub Explosion.draw()
  STATIC Float interval=TIMER
  LOCAL Integer t,c,r,x,y,nextFrame
    
  if TIMER-interval>EXPLOSIONS_SPEED then 
    interval=TIMER
    nextFrame=1
  endif

  for t=0 to EXPLOSIONS_MAX-1
    if Explosion.frame(t)>0 then
      c=Explosion.frame(t) mod 8
      r=int(Explosion.frame(t) / 8)
      x=Explosion.X(t)
      y=Explosion.Y(t)   '-Explosion.frame(t)
      blit 240+c*Explosion.W,r*Explosion.H,x,y,Explosion.W,Explosion.H,PAGE_SPRITES,&B100
      Explosion.frame(t)=(Explosion.frame(t)+nextFrame) AND 31
    endif
  next
end sub


