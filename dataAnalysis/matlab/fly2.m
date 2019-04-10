clear;
clc;
uav1 = LoadDataPosition('2017-03-20T20-54-08.txt');
m = uav1.len;
t = 1:m;
[temp1,temp2] = find((uav1.motorPwm(:,1))'>1000);
tStart = temp2(1);
tEnd = temp2(length(temp2));
%t = 1:tEnd;
t = tStart:tEnd;

% figure(1);
% clf;
% subplot(2,1,1);
% % plot(t,uav1.positionX(t,1), 'b-', t, uav1.positionX(t,2),'g-',  t, uav1.positionX(t,3)/10,'r-');
% % subplot(3,1,2);
% plot(t,uav1.angleRoll(t,1), 'b-', t, uav1.angleRoll(t,2),'g-',  t, uav1.angleRoll(t,3)/10,'r-');
% subplot(2,1,2);
% plot(t,uav1.angularRoll(t,1), 'b-', t, uav1.angularRoll(t,2),'g-',  t, uav1.angularRoll(t,3)/10,'r-');
% figure(2);
% clf;
% subplot(2,1,1);
% % plot(t,uav1.positionY(t,1), 'b-', t, uav1.positionY(t,2),'g-',  t, uav1.positionY(t,3)/10,'r-');
% % subplot(3,1,2);
% plot(t,uav1.anglePitch(t,1), 'b-', t, uav1.anglePitch(t,2),'g-',  t, uav1.anglePitch(t,3)/10,'r-');
% subplot(3,1,2);
% plot(t,uav1.angularPitch(t,1), 'b-', t, uav1.angularPitch(t,2),'g-',  t, uav1.angularPitch(t,3)/10,'r-');
% figure(3);
% clf;
% subplot(2,1,1);
% % plot(t,uav1.positionZ(t,1), 'b-', t, uav1.positionZ(t,2),'g-',  t, uav1.positionZ(t,3)/10,'r-');
% % subplot(3,1,2);
% plot(t,uav1.angleYaw(t,1), 'b-', t, uav1.angleYaw(t,2),'g-',  t, uav1.angleYaw(t,3)/10,'r-');
% subplot(2,1,2);
% plot(t,uav1.angularYaw(t,1), 'b-', t, uav1.angularYaw(t,2),'g-',  t, uav1.angularYaw(t,3)/10,'r-');


%subplot(2,1,1);
error = (uav1.positionZ(:,1) - uav1.positionZ(:,3)) ;
for i=1:m
    if error(i)>100;
        error(i) = 100;
    else if error(i) < -100
        error(i) = -100;
        end
    end
end
% con = error * 0.29  + uav1.positionZ(:,2) * 1.18;
 
% subplot(2,1,2);
 oil = mean(uav1.motorPwm,2)-4000;
x = ones(m,1) * 1100;
figure(1);
clf;
kp = 3;
ki = 0.5;
kd = 5;
kg = 0.6;
error = (uav1.positionZ(:,1) - uav1.positionZ(:,2))*0.1;
kpCon = kp*error;
kiCon = ki*uav1.velocityZ(:,3)*0.1;
error2 = [error(2:m);0];
kdCon = kd*error2;
kgCon = kg*uav1.positionZ(:,3)*0.1;
%oil   =   sum(uav1.motorPwm,2);
oil = mean(uav1.motorPwm,2)-1000;
figure(1);
clf;
uav1.velocityX(:,2) = uav1.velocityX(:,2)/10-130;
%plot(t,uav1.positionZ(t,1), 'b-', t, uav1.positionZ(t,2),'g-', t,uav1.positionZ(t,3)/10,'r-', t, uav1.velocityX(t,2),'c-', t, oil(t),'m-');
plot(t,uav1.positionZ(t,1), 'b-', t, uav1.positionZ(t,2),'g-', t, uav1.velocityZ(t,3),'r-');
axis([tStart, tEnd,0,+2000]);
% gravityI * uavData.posPid[1][1] * 0.1f
%         + uavData.posPid[2][0] * error + uavData.posPid[2][1] * positionI[2] * 0.1f + uavData.posPid[2][2] * (error - positionErrLast[2]);
% legend('ref','measure','special');
figure(2);
clf;
%plot(t,oil(t),'b-',t,uav1.joystickPwm(t,3),'g-',t,x(t),'r-');
kpValue = (uav1.velocityZ(t,1) - uav1.velocityZ(t,2)) * 0.1;
plot(t,uav1.velocityZ(t,1), 'b-', t, uav1.velocityZ(t,2),'g-',  t, uav1.velocityZ(t,3),'r-',t,uav1.statue(t)*30,'m-');%t,uav1.joystickPwm(t,3)/50,'m-');%t,uav1.statue(t)*30,'m-');
%plot(t,uav1.velocityZ(t,1), 'b-', t, uav1.velocityZ(t,2),'g-',  t, uav1.velocityZ(t,3)/100,'r-',t,uav1.velocityZ(t,1)-uav1.velocityZ(t,2),'-m');%,t , uav1.velocityZ(t,1)-uav1.velocityZ(t,2), 'c-');
axis([tStart,tEnd, -100,+100]);

figure(3);
clf;
subplot(2,1,1);
%滑模面误差-速度环
plot(t,uav1.positionZ(t,1),'b-',t,uav1.positionX(t,3),'-g');
subplot(2,1,2);
plot(t,uav1.velocityZ(t,1),'b-',t,uav1.positionY(t,3),'-g');
%plot(t,uav1.positionX(t,3),'b-');
% 比例项， 前馈项， 积分
%plot(t,uav1.positionZ(t,1)-uav1.positionZ(t,2),'-b',t,(uav1.velocityZ(t,1)-uav1.velocityZ(t,2)),'-g',t, uav1.velocityZ(t,3),'r-',t,uav1.positionZ(t,3),'m-');

% figure(4);
% clf;
% plot(t,uav1.positionX(t,3),'m-');

%,t,uav1.positionZ(t,1)-uav1.positionZ(t,2),'-m');
% figure(3)
% clf
% plot(t,uav1.positionZ(t,2)/600, 'b-', t, uav1.velocityZ(t,2)/3,'g-',t,oil(t)/500,'r-',t,uav1.velocityZ(t,3)/2000,'m-');
% axis([tStart, tEnd,-2,+2]);
% 
% figure(4)
% clf
% plot( t, oil(t),'m-');
% plot(t,uav1.velocityX(t,2), 'b-', t, uav1.velocityY(t,2),'g-',  t, uav1.velocityZ(t,2),'r-');
%plot(uav1.para(t,2));
% temp2 = [uav1.positionZ(2:m,2); 0];
% temp2 = temp2 - uav1.positionZ(:,2);
% plot(t,temp2(t));