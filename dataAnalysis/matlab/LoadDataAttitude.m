function [attitudeData] = LoadDataAttitude(fileName)
data = load(fileName);
attitudeData.statue = data(:,1);
attitudeData.powerVotage = data(:,2);
attitudeData.motorPwm = data(:,3:6);
attitudeData.joystickPwm = data(:,7:10);
attitudeData.angularRoll = data(:,11:13);
attitudeData.angularPitch = data(:,14:16);
attitudeData.angularYaw = data(:,17:19);
attitudeData.angleRoll = data(:,20:22);
attitudeData.anglePitch = data(:,23:25);
attitudeData.angleYaw = data(:,26:28);
attitudeData.para = data(:,29:31);
attitudeData.len = length(data);
end