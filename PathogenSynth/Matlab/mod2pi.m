function [y] = mod2pi(x)
% MOD2PI - Reduce x to the range [-pi,pi)
  y=x;
  twopi = 2*pi;
  while y >= pi, y = y - twopi; end
  while y < -pi, y = y + twopi; end

