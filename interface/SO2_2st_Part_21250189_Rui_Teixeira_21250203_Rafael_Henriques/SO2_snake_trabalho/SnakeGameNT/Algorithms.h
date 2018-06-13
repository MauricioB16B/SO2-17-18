#pragma once

int RangedRandDemo(int range_min, int range_max);

bool getNeighborCords(int *xcords, int *ycords, const int x, const int y);

bool get4NeighborCords(int *xcords, int *ycords, const int x, const int y);

int distanceBetween2Points(int x1, int y1, int x2, int y2);

int avoidAgainstWall(int x, int y, int currentDir, int fieldSizeX, int fieldSizeY);

int randomObj();
