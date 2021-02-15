import bpy
from mathutils import *
from math import *

#2D Vector (x y)
class Vector():
    x = 0
    y = 0
    
    #Constuctor
    #For copy constuctor, do Vector( [other.x, other.y] )
    def __init__(self, initX=0, initY=0):
        if type(initX) == Vector:
            self.x = initX.x
            self.y = initX.y
        else:
            self.x = initX
            self.y = initY

    #Sum of vectors
    def __add__(self, other):
        return Vector(self.x+other.x, self.y+other.y)
    
    #Subtraction of vectors
    def __sub__(self, other):
        return Vector(self.x-other.x, self.y-other.y)
    
    #Vector multiplied by scalar
    #int
    def __mul__(self, scalar):
        return Vector(self.x*scalar, self.y*scalar)
    #float
    def __rmul__(self, scalar):
        return self.__mul__(scalar)
    
    #Vector divided by scalar
    #int
    def __div__(self, scalar):
        return Vector(self.x/scalar, self.y/scalar)
    #float
    def __rdiv__(self, scalar):
        return self.__div__(scalar)
    
    def module(self):
        return sqrt(self.x**2+self.y**2)
    
    def angle(self):
        if self.x == 0:
            if self.y > 0:
                return pi/2
            elif self.y < 0:
                return 3*pi/2
            else:
                return 0
        else:
            if self.x > 0:
                return atan( self.y/self.x )
            else:
                return pi+atan( self.y/self.x )

class Particle:
    pos = Vector()
    charge = 0
    
    #Constuctor, or = sign
    def __init__(self, position=Vector(), charge=0):
        self.pos = position
        self.charge = charge

#Scaling variables

#xy-axis scale. Depends on scale of object
#(0,1) is the local coordinates of the object
scale = 1/60

#Electric force constant is redifined as the z-axis scale
#the actual constant, 9000000000, is too much
K = scale


#Virtual particles
#Needs manual update to update with actual particles
part1 = Particle( Vector(0, 30*scale), 10 )
part2 = Particle( Vector(0, -30*scale), -10 )

def scalarField():
    plane = bpy.data.objects['Plane']
    planeVerts = plane.data.vertices
    for v in planeVerts:
        planePos = Vector( v.co[0], v.co[1] )
        
        tempPos = Vector( v.co[0], v.co[1] )
        
        
        r1 = tempPos-part1.pos
        r2 = tempPos-part2.pos
        
        if (r1.module() != 0) and (r2.module() != 0):
            #Vector can't divide by float and I don't know why
            
            temp1 = K*part1.charge/r1.module()
            temp2 = K*part2.charge/r2.module()
            field = temp1+temp2
            
            if field > 0.5:
                field = 0.5
            elif field < -0.5:
                field = -0.5

        else:
            if r1.module() < r2.module():
                field = 0.5*part1.charge/abs(part1.charge)
            elif r1.module() > r2.module():
                field = 0.5*part2.charge/abs(part2.charge)
            else:
                field = 0
        
        v.co = (
            planePos.x,
            planePos.y,
            field
            )
    
def vectorField():
    
    fieldVectors = bpy.data.collections['Field vectors'].all_objects
    for i in range(0, 7):
        for j in range(0, 7):
            ID = 7*i+j
            
            pointPos = Vector(
                fieldVectors[ID].location[0],
                fieldVectors[ID].location[1]
            )
            
            r1 = pointPos-part1.pos
            r2 = pointPos-part2.pos
            
            #Vector can't divide by float and I don't know why
            if (r1.module() != 0) and (r2.module() != 0):
                field1 = r1*( part1.charge/r1.module()**3 )
                field2 = r2*( part2.charge/r2.module()**3 )
                field = field1+field2
                fieldVectors[ID].rotation_euler[2] = field.angle()
            else:
                fieldVectors[ID].rotation_euler[2] = 0

def reset():
    plane = bpy.data.objects['Plane']
    planeVerts = plane.data.vertices
    for v in planeVerts:
        planePos = Vector( v.co[0], v.co[1] )
        
        v.co = (planePos.x, planePos.y, 0.0)

    fieldVectors = bpy.data.collections['Field vectors'].all_objects
    for i in range(0, 7):
        for j in range(0, 7):
            ID = 7*i+j
            fieldVectors[ID].rotation_euler[2] = 0

scalarField()
vectorField()
#reset()