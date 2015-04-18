% rnd_assoc_gui.m
% This program constructs a 3-layer random autoassociator using the 'network', 
% 'layer', and 'unit' objects. It is very similar to rnd_assoc.m, but it
% has a graphical interface.

clear;
close all;

%% 0) Create a figure to visualize the simulation
global rtype run
% These are the variables that the GUI uses to communicate with this script
% rtype = type of simulation being done.
%         1 -> cycle, 2 -> trial, 3 -> epoch, 4 -> just go
% run = 0 -> don't run now, 1 -> run

f = create_window;  % creates figure, buttons, callback functions

h_inp = subplot(2,3,1);  % handle to the axis with activity of input layer
h_hid = subplot(2,3,2);  % handle to the axis with activity of hidden layer
h_out = subplot(2,3,3);  % handle to the axis with activity of output layer
h_err = subplot(2,3,4);  % handle to axis with mean errors by epoch
h_wh = subplot(2,3,5);  % handle to axis with hidden layer weights
h_wo = subplot(2,3,6);  % handle to axis with output layer weights

%% 1) First, specify the dimensions and connectivity of the layers
%  At this point, layers are either fully connected or unconnected

% Set the dimensions of the layers
dim_lays = {[5 5],[5 5],[5 5]}; % specifying 3 layers and their dimensions

% Specify connectivity between layers
connections = [0  0  0;
               1  0 .2;
               0  1  0];
   % connections(i,j) = c means that layer i receives connections from j,
   % and that they have a relative strength c. In this case, feedback
   % connections are 5 times weaker. The relative weight scale of layer i
   % comes from the non-zero entries of row i.   
   % The network constructor will normalize this matrix so that if there
   % are non-zero entries in a row, they add to 1.
   
%% 2) Now specify the initial weight matrices
n_lays = length(dim_lays);  % number of layers
n_units = zeros(1,n_lays); % number of units in each layer
for i = 1:n_lays
    n_units(i) = dim_lays{i}(1)*dim_lays{i}(2);
end
w0 = cell(3); % this cell will contain all the initial connection matrices
for rcv = 1:n_lays
    for snd = 1:n_lays
        if connections(rcv,snd) > 0
            % random initial weights between close to 0.5, because of
            % weight contrast enhancement
            w0{rcv,snd} = 0.4 + 0.2*rand(n_units(rcv),n_units(snd));
            % notice that the dimensions of the layer don't matter, only
            % the number of units. Layers are 2-dimensional only for
            % purposes of visualization.
        end
    end
end

%% 3) Create the network using the constructor
net = network(dim_lays, connections, w0);

%% 4) Let's create some inputs
n_inputs = 5;  % number of input-output patterns to associate
patterns = cell(n_inputs,2); % patterns{i,1} is the i-th input pattern, and 
                             % patterns{i,2} is the i-th output pattern.
% This will assume that layers 1 and 3 are input and output respectively.
% Pattern values are binary, either 0.01 or 0.99
patterns{1,1} = repmat([0 0 1 0 0],5,1);   % vertical line
patterns{2,1} = [1 1 1 1 1;zeros(4,5)];   % horizontal line
patterns{3,1} = [0 0 0 0 1;0 0 0 1 0;0 0 1 0 0;0 1 0 0 0; 1 0 0 0 0]; % diagonal 1
patterns{4,1} = patterns{3,1}([5 4 3 2 1],:);   % diagonal 2
patterns{5,1} = patterns{3,1}|patterns{4,1};   % two diagonals
for i = 1:n_inputs % outputs are the same as inputs (an autoassociator)
    patterns{i,1} = 0.01 + 0.98*patterns{i,1};
    patterns{i,2} = patterns{i,1};
end
    
%% 5) Train the network

% Specify parameters for training
n_epochs = 15;  % number of epochs. All input patterns are presented in one.
n_trials = n_inputs; % number of trials. One input pattern per trial.
n_minus = 50;  % number of minus cycles per trial.
n_plus = 25; % number of plus cycles per trial.
net.lrate = 0.5; % learning rate

% some stuff to plot errors
errors = zeros(n_epochs,n_trials); % cosine error for each pattern
mean_errs = zeros(1,n_epochs);
hp_err = plot(h_err,mean_errs); title(h_err,'cosine error'); xlabel(h_err,'epoch');
% using hp_err you can change the data in the plot without redrawing the
% axis of h_err. A bit faster than replotting.

pause on; % the command 'pause' will stop Matlab
while run == 0 || isempty(rtype)
    pause(0.2); % waiting until RUN/STOP is pressed, which sets run = 1
end

epoch = 0;
while epoch < n_epochs && run
    epoch = epoch + 1;
    trial = 0;
    order = randperm(n_trials); % order of presentation of inputs this epoch
 
    while trial < n_trials && run
        trial = trial + 1;
        net.reset;  % randomize the acts for all units        
        pat = order(trial);  % input to be presented this trial
        %++++++ MINUS PHASE +++++++        
        inputs = {patterns{pat,1},[],[]};
        minus = 0;
        while minus < n_minus && run % minus cycles: layer 1 is clamped
            minus = minus + 1;
            net.cycle(inputs,1);             
            
            % end of cycle check
            if rtype == 1  % we need to stop at the end of a cycle
                run = 0;
                %------ visualizing minus phase each cycle ------
                hidden = net.layers{2}.activities; % for plotting
                inp_pat = net.layers{1}.activities;
                bar3(h_inp,reshape(inp_pat,dim_lays{1}(1),dim_lays{1}(2)));                
                zlim(h_inp,[0 1]); title(h_inp,'L1 act');
                bar3(h_out,reshape(net.layers{3}.activities',dim_lays{3}(1),dim_lays{3}(2)));
                zlim(h_out,[0 1]); title(h_out,'L3 act');
                bar3(h_hid,reshape(hidden,dim_lays{2}(1),dim_lays{2}(2)));
                zlim(h_hid,[0 1]); title(h_hid,'L2 act');
                drawnow;                
            end
            while run == 0
                pause(0.2);
            end            
        end
        outs = net.layers{3}.activities'; % saving the output for testing
        
        %------ visualizing minus phase each trial ------
        hidden = net.layers{2}.activities; % for plotting
        inp_pat = net.layers{1}.activities;
        bar3(h_inp,reshape(inp_pat,dim_lays{1}(1),dim_lays{1}(2)));
        %bar3(h_inp,patterns{pat,1}); % input pattern
        zlim(h_inp,[0 1]); title(h_inp,'L1 act');
        bar3(h_out,reshape(outs,dim_lays{3}(1),dim_lays{3}(2)));
        zlim(h_out,[0 1]); title(h_out,'L3 act');
        bar3(h_hid,reshape(hidden,dim_lays{2}(1),dim_lays{2}(2)));
        zlim(h_hid,[0 1]); title(h_hid,'L2 act');
        drawnow;        
        
        %+++++++ PLUS PHASE +++++++
        inputs = {patterns{pat,1},[],patterns{pat,2}};
        plus = 0;
        while plus < n_plus && run % plus cycles: layers 1 and 3 are clamped
            plus = plus + 1;
            net.cycle(inputs,1);
            
            % end of cycle check
            if rtype == 1  % we need to stop at the end of a cycle
                run = 0;
                
                %------ visualizing plus phase each cycle ------
                hidden = net.layers{2}.activities; % for plotting                
                bar3(h_hid,reshape(hidden,dim_lays{2}(1),dim_lays{2}(2)));
                zlim(h_hid,[0 1]); title(h_hid,'L2 act');
                drawnow;
            end
            while run == 0
                pause(0.2);
            end
        end
        
        net.updt_long_avgs; % update averages used for net input scaling                            
        
        %------ visualizing plus phase each trial ------
        hidden = net.layers{2}.activities; % for plotting
        inp_pat = net.layers{1}.activities;
%         bar3(h_inp,reshape(inp_pat,dim_lays{1}(1),dim_lays{1}(2)));
%         zlim(h_inp,[0 1]); title(h_inp,'L1 act');
%         out_pat = net.layers{3}.activities;
%         bar3(h_out,reshape(out_pat,dim_lays{3}(1),dim_lays{3}(2)));       
%         zlim(h_out,[0 1]);
        bar3(h_hid,reshape(hidden,dim_lays{2}(1),dim_lays{2}(2)));
        zlim(h_hid,[0 1]); title(h_hid,'L2 act');
        drawnow;
        
        %+++++++ LEARNING +++++++
        net.XCAL_learn; % udates the avg_l vars and applies XCAL learning
        if mod(trial,10) == 0
            disp(['trial ', num2str(trial), ' finished'])
        end
        
        %------ plotting some weight matrices ------
        % bar3(h_wi,net.layers{1}.wt); % layer 1 is unconnected
        bar3(h_wh,net.layers{2}.ce_wt); title(h_wh,'L2 ce weights');
        bar3(h_wo,net.layers{3}.ce_wt); title(h_wo,'L3 ce weights');
        %drawnow;
        
        %+++++++ ERRORS +++++++
        % Only the cosine error is used here
        errors(epoch,pat) = 1 - sum(outs.*patterns{pat,2}(1:end)) / ...
            ( norm(outs)*norm(patterns{pat,2}(1:end)) );
        
        % end of trial check
        if rtype < 3  % we need to stop at the end of a trial
            run = 0;
        end
        while run == 0;
            pause(0.5);
        end
    end
    
    % display mean errors of epoch
    mean_errs(epoch) = mean(errors(epoch,:));
    set(hp_err,'YData',mean_errs(1:epoch));  % a bit faster than replottng
    %plot(h_err,1:epoch,mean_errs(1:epoch));
    drawnow;
    disp(['epoch ', num2str(epoch), ' finished']);
    
    % end of epoch check
    if rtype < 4  % we need to stop at the end of an epoch
        run = 0;
    end
    while run == 0
        if rtype == 4
            disp('Press RUN/STOP and type "return" to continue');
            keyboard; % this allows to pause the simulation and enter 
                      % keyboard commands until 'return' is typed
        else
            pause(0.5);
        end
    end
end

%% 6) Plotting again the cosine error per epoch
mean_errs = mean(errors,2);
figure;
plot(mean_errs);

