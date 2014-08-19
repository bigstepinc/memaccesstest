function plot_csv_log(file)
csv=csvread(file);
[csv,i]=sortrows(csv,1);

max_index=max(csv(1:end,1));

clf;
hold on;

for i=0:max_index,

s=size(csv);
sx=s(1,1);

A=csv(csv(:,1)==i, 2:end);

%prepare xstamp
x=A(1:end,1);
min_x=min(x);
max_x=max(x);
x=x.-min_x;
%x=x(1:plot_every:end);

%prepare yonse xs

y=A(1:end,2);
%y=csv(1:end,5)/1000; %submillisecond resolution if supported by sampler
%y=rmoutlier(y,1,0,0);
%y=y;
%y=y(1:plot_every:end);

count=A(1:end,3);

y2=y./count

%plot data

%semilogx(x,y,'.');
plot(log2(x),y2);

end;

ax = gca();
set(ax, 'outerposition', [0.1, 0.1, 0.8, 0.8]);
%ylabel('time (s) lower is better');
%xlabel('request time (s)');
%set(ax,'xlim',[x(1,1),x(end,1)]);
%legend(['rsp';'trnd']);

%title(["Plot of dataset: " file " every " int2str(plot_every) "th request"]);
%print(strcat(file,".eps"),"-deps");


