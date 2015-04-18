function f = create_window
% f = create_window creates the figure and buttons required by the GUI in
% rnd_assoc_gui.m. It returns the handle to that figure.

global rtype run

f = figure('Position',[250,400,800,500]);

% creating buttons 
hmenu = uicontrol('Style','popupmenu',...
           'String',{'cycle','trial','epoch','all epochs'},...
           'Position',[270,10,100,25],'Callback',@menu_call);
hrun = uicontrol('Style','togglebutton','String','RUN/STOP',...
           'Position',[450,10,100,25],'Callback',@run_call);

% this is so the buttons change size when the window is resized
set(f,'Units','normalized');
set(hmenu,'Units','normalized');
set(hrun,'Units','normalized');
set(f,'Name','Happy Window');

% this is so the buttons react during a simulation
set(hmenu,'Interruptible','off');
set(hrun,'Interruptible','off');

% setting the default values for the simulation
rtype = 4;
run = 0;
%set(hmenu,'String','all epochs');

% creating callback functions
    function menu_call(source,eventdata)
        % Determine the selected type of simulation
        str = source.String;
        val = source.Value;
        % Set current value of rtype to the selected type of sim
        switch str{val};
            case 'cycle'
                rtype = 1;
            case 'trial'
                rtype = 2;
            case 'epoch'
                rtype = 3;
            case 'all epochs'
                rtype = 4;
        end
    end

    function run_call(source,eventdata)
        val = source.Value;
        if val == 0;
            run = 0;  % stop simulating
            disp('Select simulation length and press RUN/STOP');
        else
            run = 1;  % simulate
        end
    end
end