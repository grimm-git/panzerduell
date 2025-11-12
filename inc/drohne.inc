
CONST DROHNE_ROTSPD=50
CONST DROHNE_MOVSPD=15

CONST DROHNE_OFF=0
CONST DROHNE_HIGH=1
CONST DROHNE_LOW=2

DIM Integer Drohne.W=23
DIM Integer Drohne.H=23

DIM Integer Drohne.State
DIM Float   Drohne.X
DIM Float   Drohne.Y
DIM Float   Drohne.phi      'in DEG
DIM Float   Drohne.targetX
DIM Float   Drohne.targetY
DIM Float   Drohne.tiAnim

DIM Float   Drohne.dt
DIM Float   Drohne.Px(3)  'Bezier curve 3rd degree
DIM Float   Drohne.Py(3)


sub Drohne.init()
  Drohne.state=DROHNE_OFF
end sub

'Start Drohne on an autonomous flight to target (x2!,y2!)
sub Drohne.start(x2!,y2!)
  STATIC Float sx(3)=(Screen.VPx+VP.W/2,Screen.VPx+VP.W/2,Screen.VPx,Screen.VPx+VP.W)
  STATIC Float sy(3)=(Screen.VPy,Screen.VPy+VP.H,Screen.VPy+VP.H/2,Screen.VPy+VP.H/2)
  LOCAL Float x1,y1
  LOCAL Float nx,ny,nl
  LOCAL Float a,b,d,s
  LOCAL Integer idx

  if Drohne.state=DROHNE_OFF then
    idx=int(RND()*4) : x1=sx(idx) : y1=sy(idx)  'random start position

    if x2!=x1! then inc x1!,0.00001 'prevent division by zero
    nx=y1!-y2! : ny=x2!-x1! : nl=SQR(nx*nx+ny*ny)
    nx=nx/nl   : ny=ny/nl 'Normed normal vector
    
    Drohne.dt=3/Math.distance(x1!,y1!,x2!,y2!)

    a=(y2!-y1!)/(x2!-x1!) : b=y1!-a*x1! : d=(x2!-x1!)/3
    s=choice(y1!<Screen.VPy+VP.H/2,-1,1)  

    Drohne.Px(0)= x1! : Drohne.Py(0)=y1! 'start
    Drohne.Px(1)=  (x1!+d)     +s * nx * (75+50*RND())
    Drohne.Py(1)=a*(x1!+d)+b   +s * ny * (75+50*RND())  'Bezier point 1
    Drohne.Px(2)=  (x1!+2*d)   -s * nx * (75+50*RND())
    Drohne.Py(2)=a*(x1!+2*d)+b -s * ny * (75+50*RND())  'Bezier Point 2
    Drohne.Px(3)= x2! : Drohne.Py(3)=y2! 'target
    Drohne.State=DROHNE_HIGH
    Drohne.tiAnim=0    
  endif
end sub

sub Drohne.move(x!,y!,phi!)
  if Drohne.state=DROHNE_OFF then exit sub

  Drohne.X=x!
  Drohne.Y=y!
  Drohne.phi=phi!
end sub

sub Drohne.update()
  STATIC Float tiMove=0,t=0
  LOCAL Float x,y,rect(7)
  LOCAL Integer idx

  if Drohne.State=DROHNE_OFF then exit sub
  if TIMER-tiMove>DROHNE_MOVSPD then
    tiMove=TIMER : inc t,Drohne.dt
    if t>1 then
      t=0 : Drohne.State=DROHNE_OFF

      _makeRectangle rect(),Drohne.X,Drohne.Y,Drohne.W,Drohne.H,RAD(Drohne.phi)
      for idx=0 to PANZER_MAX-1
        if Panzer.hitByPanzer(rect(),idx) then
          playSample 14,22050,2
          Panzer.destroy idx
          Player.kill idx+1
          exit sub
        endif
      next

      Explosion.add Drohne.X,Drohne.Y,2,EX_TYPE_SMALL
      exit sub
    else if t>0.9 then
      Drohne.State=DROHNE_LOW
    endif

    Drohne.X=_bezier_x(t)
    Drohne.Y=_bezier_y(t)

    x=_bezier_x(t+Drohne.dt)
    y=_bezier_y(t+Drohne.dt)
    Drohne.phi=atan2(x-Drohne.X,-(y-Drohne.Y))/PI*180
  endif
end sub

sub Drohne.draw()
  STATIC Integer rotor=0
  LOCAL Float x,y,phi,w,h
  LOCAL Integer high
  
  if Drohne.State=DROHNE_OFF then exit sub
  if TIMER-Drohne.tiAnim>DROHNE_ROTSPD then Drohne.tiAnim=TIMER : rotor=rotor XOR 1

[DRN] for x=0 to 3 : circle Drohne.Px(x),Drohne.Py(x),2,,,map(RED),map(RED) : next
[DRN] for x=0 to 1 step 0.01 : pixel _bezier_x(x),_bezier_y(x),map(WHITE) : next
  
  x=Drohne.X : y=Drohne.Y
  inc x,-Drohne.W/2 : inc y,-Drohne.H/2 : phi=Drohne.phi : w=Drohne.W : h=Drohne.H
  if isVisible(x,y) then
    high=choice(Drohne.State=DROHNE_HIGH,0,2)
    image rotate_fast 128,(high+rotor)*h,w,h,x,y,phi,PAGE_SPRITES,1
    Drohne.playSound()
  end if
end sub

sub Drohne.playSound()
  STATIC Float tim=TIMER

  if TIMER-tim>2000 then
    tim=TIMER
    playSample 13,22050,2
  endif
end sub

Function _bezier_x(t!) as Float
  LOCAL Float x

  x =     (1-t!)^3        * Drohne.Px(0)
  inc x,3*(1-t!)^2 * t!   * Drohne.Px(1)
  inc x,3*(1-t!)   * t!^2 * Drohne.Px(2)
  inc x,     t!^3         * Drohne.Px(3)
  _bezier_x=x
end function

Function _bezier_y(t!) as Float
  LOCAL Float y

  y =     (1-t!)^3        * Drohne.Py(0)
  inc y,3*(1-t!)^2 * t!   * Drohne.Py(1)
  inc y,3*(1-t!)   * t!^2 * Drohne.Py(2)
  inc y,     t!^3         * Drohne.Py(3)
  _bezier_y=y
end function


