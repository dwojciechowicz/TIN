//normalDistribution.c
//Autor: Magdalena Zych
//Data: 25.04.2020

#include "normalDistribution.h"

float normalDistribution(float mean, float std)
{
  float u1 = rand() / (RAND_MAX + 1.0);
  float u2 = rand() / (RAND_MAX + 1.0);

  float z = sqrt(-2 * log(u1)) * cos(2 * M_PI * u2);

  return (mean + z * std);
}
