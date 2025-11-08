'Various mathematical function

sub Math.drawNormal(X!,Y!,vX!,vY!,x1!,y1!,x2!,y2!)
  LOCAL Float m1,b1,m2,b2
  LOCAL Float nix,niy
  LOCAL Float nx,ny,nl

  if x2!=x1! then x2!=x2!+0.000001  'trick "Division by zero"
  nx=y1!-y2! : ny=x2!-x1! : nl=SQR(nx*nx+ny*ny)
  nx=nx/nl   : ny=ny/nl   'normed normal vector, start at ball position

  if nx=0 then nx=0.000001  'trick "Division by zero"
  m1=ny/nx : b1=Y!-m1*X!    'ball-panel line

  m2=(y2!-y1!)/(x2!-x1!) : b2=y1!-m2*x1! 'current block surface
  nix=(b2-b1)/(m1-m2)    : niy=m1*nix+b1 'Intersection point current surface

  line X!,Y!,nix,niy,1,rgb(blue)
  circle nix,niy,2,0,1,rgb(red),rgb(red)
end sub

'(x1/y1),(x2,y2)=Ball vector
'(x3/y3),(x4,y4)=Panel surface
function Math.intersect(x1!,y1!,x2!,y2!,x3!,y3!,x4!,y4!) as Integer
  LOCAL Float den,ua,ub,numA,numB

  den=(y4!-y3!)*(x2!-x1!) - (x4!-x3!)*(y2!-y1!)
  if den=0 then exit function
  
  numA=(x4!-x3!)*(y1!-y3!) - (y4!-y3!)*(x1!-x3!)
  numB=(x2!-x1!)*(y1!-y3!) - (y2!-y1!)*(x1!-x3!)
  ua=numA/den : ub=numB/den

  if ua>=0 and ua<=1 and ub>=0 and ub<=1 then Math.intersect=1
end function

function Math.distanceToLine(px!,py!,x1!,y1!,x2!,y2!) as float
  LOCAL float dx,dy,length
  LOCAL float t,projX,projY
  
  dx=x2!-x1!: dy=y2!-y1!: length=dx*dx+dy*dy
  if length=0 then
    Math.distanceToLine=SQR((px!-x1!)^2 + (py!-y1!)^2)
    exit function
  endif

  t=((px!-x1!)*dx + (py!-y1!)*dy) / length
  if t<0 then t=0
  if t>1 then t=1

  projX=x1!+t*dx : projY=y1!+t*dy
  Math.distanceToLine=SQR((px!-projX)^2 + (py!-projY)^2)
end function

function Math.distanceToLine2(px!,py!,x1!,y1!,x2!,y2!) as float
  LOCAL float dx,dy,length
  LOCAL float t,projX,projY
  
  dx=x2!-x1!: dy=y2!-y1!: length=dx*dx+dy*dy
  if length=0 then
    Math.distanceToLine2=SQR((px!-x1!)^2 + (py!-y1!)^2)
  else  
    t=((px!-x1!)*dx + (py!-y1!)*dy) / length
    if t<0 then t=0
    if t>1 then t=1

    projX=x1!+t*dx : projY=y1!+t*dy
    Math.distanceToLine2=SQR((px!-projX)^2 + (py!-projY)^2)
  endif
end function


function Math.crossProduct(px!,py!,x1!,y1!,x2!,y2!) as Float
  Math.crossProduct=(y2!-y1!)*(px!-x1!)-(x2!-x1!)*(py!-y1!)
end function

'rotates a line (x1,y1)(x2,y2) by angle phi
sub Math.rotate(phi!,x1!,y1!,x2!,y2!)
  LOCAL Float x,y,cx,cy
  
  cx=x1!+(x2!-x1!)/2
  cy=y1!+(y2!-y1!)/2  'calculate center coords

  x=x1!-cx : y=y1!-cy 'move line to (0/0)
  x1!=cx + x*cos(phi!) - y*sin(phi!) 'rotate and move it back into position
  y1!=cy + x*sin(phi!) + y*cos(phi!)

  x=x2!-cx : y=y2!-cy 'move line to (0/0)
  x2!=cx + x*cos(phi!) - y*sin(phi!) 'same with the second point
  y2!=cy + x*sin(phi!) + y*cos(phi!)
end sub




