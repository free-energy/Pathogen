function saveplot(filename)
% SAVEPLOT - Save current plot to disk
      
cmd = ['print -depsc ',filename];
% For PDF format: 
%     cmd = ['print -dpdf ',filename];
% For PNG format:
%     cmd = ['print -dpng ',filename]; % .png graphics format
% For monochrome PostScript format: 
%     cmd = ['print -deps ',filename];
% For color PostScript format: 
%     cmd = ['print -depsc ',filename];
% Etc. - say 'help print' in either Matlab or Octave
disp(cmd); eval(cmd);