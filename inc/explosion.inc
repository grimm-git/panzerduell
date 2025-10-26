' Explosion code

CONST EXPLOSIONS_MAX=5
CONST EXPLOSIONS_SPEED=30

CONST EX_TYPE_SMALL=0
CONST EX_TYPE_BIG=1

DIM Integer Explosion.W=50
DIM Integer Explosion.H=49
DIM Integer Explosion.frame(EXPLOSIONS_MAX)
DIM Integer Explosion.type(EXPLOSIONS_MAX)
DIM Float   Explosion.X(EXPLOSIONS_MAX)
DIM Float   Explosion.Y(EXPLOSIONS_MAX)

DIM Integer ExData(1,2)

sub Explosion.init()
  LOCAL Integer t

  ExData(EX_TYPE_SMALL,0)=25  'width
  ExData(EX_TYPE_SMALL,1)=25  'height
  ExData(EX_TYPE_SMALL,2)=11  'sound sample
  ExData(EX_TYPE_BIG,  0)=50
  ExData(EX_TYPE_BIG,  1)=49
  ExData(EX_TYPE_BIG,  2)=11

  for t=0 to EXPLOSIONS_MAX-1
    Explosion.frame(t)=0
    Explosion.type(t)=EX_TYPE_BIG
    Explosion.X(t)=0
    Explosion.Y(t)=0
  next
end sub

'x!,y! in screen coordinates
sub Explosion.add(x!,y!,ch%,type%)
  LOCAL Integer t

  if type%>bound(ExData(),1) then exit sub

  for t=0 to EXPLOSIONS_MAX-1
    if Explosion.frame(t)=0 then
      Explosion.frame(t)=1
      Explosion.type(t)=type%
      Explosion.X(t)=x!-ExData(type%,0)/2
      Explosion.Y(t)=y!-ExData(type%,1)/2
      PlaySample ExData(type%,2),22050,ch%
      exit sub
    endif
  next
end sub

sub Explosion.draw()
  STATIC Float interval=TIMER
  LOCAL Integer t,c,r,x,y,nextFrame
  LOCAL Integer h,w
    
  if TIMER-interval>EXPLOSIONS_SPEED then 
    interval=TIMER
    nextFrame=1
  endif

  for t=0 to EXPLOSIONS_MAX-1
    if Explosion.frame(t)>0 then
      x=Explosion.X(t) : y=Explosion.Y(t)

      select case Explosion.type(t)
      case EX_TYPE_SMALL
        c=Explosion.frame(t) mod 16
        r=int(Explosion.frame(t) / 16)
        w=ExData(EX_TYPE_SMALL,0) : h=ExData(EX_TYPE_SMALL,1)
        blit 240+c*w,196+r*h,x,y,w,h,PAGE_SPRITES,&B100
      case EX_TYPE_BIG
        c=Explosion.frame(t) mod 8
        r=int(Explosion.frame(t) / 8)
        w=ExData(EX_TYPE_BIG,0) : h=ExData(EX_TYPE_BIG,1)
        blit 240+c*w,r*h,x,y,w,h,PAGE_SPRITES,&B100
      end select

      Explosion.frame(t)=(Explosion.frame(t)+nextFrame) AND 31
    endif
  next
end sub


