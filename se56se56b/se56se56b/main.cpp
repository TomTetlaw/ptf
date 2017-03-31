/********************************************************
Author: Elizabeth Barker & Shani Westwood
Date: 13/03/2017
Purpose: Source Code of 3 lengths input to form a triangle
**********************************************************/
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>

int main()
{
	int s1, s2, s3;

	/* Get inputs from user */
	printf("Please enter the first side's length: \n");
	scanf("%d%*c", &s1);

	printf("Please enter the second side's length: \n");
	scanf("%d%*c", &s2);

	printf("Please enter the third side's length: \n");
	scanf("%d%*c", &s3);

	/* Check if the triangle is valid */
	if ((s1 + s3>s2) && (s2 + s3>s1) && (s2 + s3>s1) && (s1>0) && (s2>0) && (s3>0))
	{
		printf("These three lengths make a triangle!\n");
		/* Check what type of triangle it is */
		if ((s1 == s2) && (s2 == s3) && (s1 == s3))
		{
			printf("This is an equilateral triangle.");
		}
		else
			if ((s1 != s2) && (s2 != s3) && (s1 != s3))
			{
				printf("This is a scalene triangle.");
			}
			else
				if ((s1 == s2) || (s2 == s3) || (s1 == s3))
				{
					printf("This is an isoceles triangle.");
				}
	}
	/* Advise that the triangle is invalid if it is none of the above */
	else
	{
		printf("You have entered an invalid length.");
	}


	return(0);
}
