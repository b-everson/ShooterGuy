#include "Advanced2D.h"
namespace Advanced2D
{

//This file is a code snippet of a function I added to the engine used in Jonathan Harbour's book
//titled "Advanced 2D Game Development".
	
        double Math::AngleToInterceptDegrees(double x1, double y1, double x2, double y2, double xVel, double yVel, double interceptVelLength)
	{   			
		//get angle of target -> source
		double angleTargetToSource = AngleToTarget(x2,y2,x1,y1);
		angleTargetToSource = wrapAngleDegs(90 + toDegrees(angleTargetToSource));

		//get angle of target -> velocity
		double angleTargetToIntercept = AngleToTarget(x2, y2, x2 + xVel, y2 + yVel);
		angleTargetToIntercept = wrapAngleDegs(90 + toDegrees(angleTargetToIntercept));

		//get # of degrees in difference
		double degreesInterceptTargetSource = fabs(angleTargetToSource - angleTargetToIntercept);

		double targetVelocityLength = Length(xVel, yVel, 0);
		 
		double lengthRatio = targetVelocityLength  / interceptVelLength;

		if (lengthRatio > .995 && degreesInterceptTargetSource > 90)
		{
			return - 1;
		}
		
		double sinDegreesB = sin(toRadians(degreesInterceptTargetSource));

		//
		double sinOfDegreesY = lengthRatio * sinDegreesB;

		if (sinOfDegreesY > 1) 
		{
			return -1;
		}

		//get # of degrees in angle of targetSourceIntercept
                double radiansIntercept = asin(sinOfDegreesY);

		double degreesSource = toDegrees(asin(sinOfDegreesY));

 		double degreesIntercept = 180 - degreesSource - degreesInterceptTargetSource;

		double angleToTarget = AngleToTarget(x1,y1,x2,y2);
		angleToTarget = wrapAngleDegs(90 + toDegrees(angleToTarget));

		//sourceTargetDistance / sin interceptAngle = targetDistance / sin sourceAngle = targetInterceptVelocity / sin targetAngle
		double distanceOverSinAngleActual = Distance(x1,y1,x2,y2) / sin(toRadians(degreesIntercept));

		double actualTargetDistance = distanceOverSinAngleActual * sin(toRadians(degreesSource));

		Vector3 targetVelocityVectorBase = LinearVelocity(angleTargetToIntercept);

		double targetVelocityLengthBase = targetVelocityVectorBase.Length();

		double sizeMultiplier = actualTargetDistance / targetVelocityLengthBase;

		Vector3 interceptPosition = Vector3(x2 + targetVelocityVectorBase.getX() * sizeMultiplier, y2 + targetVelocityVectorBase.getY() * sizeMultiplier, 0.0f);
		
		double angleToIntercept = AngleToTarget(x1,y1,interceptPosition.getX(), interceptPosition.getY());
		angleToIntercept = wrapAngleDegs(90 + toDegrees(angleToIntercept));

		//angle to target from up - degrees of source angle in triangle of target/source/intercept
		return angleToIntercept;	
	}

	double Math::AngleToInterceptDegrees(Vector3& source, Vector3& target, Vector3& velocity, double interceptVelLength)
	{
		return AngleToInterceptDegrees(source.getX(), source.getY(), target.getX(), target.getY(), velocity.getX(), velocity.getY(), interceptVelLength);
	}
}