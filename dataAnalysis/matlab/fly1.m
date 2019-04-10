clear;
clc;
uav1 = LoadDataPosition('2017-09-18T21-38-23.txt');
m = uav1.len;
t = 1:m;
[temp1,temp2] = find((uav1.motorPwm(:,1))'>1000);
tStart = temp2(1);
tEnd = temp2(length(temp2));
t = tStart:tEnd;
t = tStart:tEnd;
figure(1);
clf;
subplot(2,1,1);
plot(t,uav1.angleRoll(t,1), 'b-', t, uav1.angleRoll(t,2),'g-',  t, uav1.angleRoll(t,3)/10,'r-');
subplot(2,1,2);
plot(t,uav1.angularRoll(t,1), 'b-', t, uav1.angularRoll(t,2),'g-',  t, uav1.angularRoll(t,3)/10,'r-');
axis([tStart, tEnd,-10,+10]);
figure(2);
clf;
subplot(2,1,1);
plot(t,uav1.anglePitch(t,1), 'b-', t, uav1.anglePitch(t,2),'g-',  t, uav1.anglePitch(t,3)/100,'r-');
subplot(2,1,2);
plot(t,uav1.angularPitch(t,1), 'b-', t, uav1.angularPitch(t,2),'g-',  t, uav1.angularPitch(t,3)/10,'r-');
axis([tStart, tEnd,-10,+10]);
figure(3);
clf;
subplot(2,1,1);
plot(t,uav1.angleYaw(t,1), 'b-', t, uav1.angleYaw(t,2),'g-',  t, uav1.angleYaw(t,3),'r-');
subplot(2,1,2);
%, t, uav1.angleYaw(t,3)/10,'m-'
plot(t,uav1.angularYaw(t,1), 'b-', t, uav1.angularYaw(t,2),'g-',  t, uav1.angularYaw(t,3),'r-');
axis([tStart, tEnd,-10,+10]);
% figure(4);
% clf;
% subplot(2,1,1);
% %plot(t,sin(uav1.angleRoll(t,1)*3.14/180),'b-',t,uav1.angleRoll(t,3),'-g');
% plot(t,uav1.angleYaw(t,1),'b-',t,uav1.anglePitch(t,3),'-g');
% subplot(2,1,2);
% %plot(t,sin(uav1.anglePitch(t,1)*3.14/180),'b-',t,uav1.anglePitch(t,3),'-g');
% plot(t,uav1.angularYaw(t,1),'b-',t,uav1.angleYaw(t,3),'-g');
% for i=1:m-1
%     uav1.angleRoll(i+1,3) = uav1.angleRoll(i,3)* 0.8 + uav1.angleRoll(i+1,3)* 0.2;
% end
%plot(t,sin(uav1.angularRoll(t,1)) - sin(uav1.angularRoll(t,2)), 'b-', t,uav1.angleRoll(t,3) ,'g-');
%  subplot(2,1,1);
%  plot(t,sin(uav1.angleRoll(t,1)),'-b',t,uav1.angleRoll(t,3)*20,'r-');
%  subplot(2,1,2);
%  plot(t,sin(uav1.anglePitch(t,1)),'-b',t,uav1.anglePitch(t,3)*20,'r-');