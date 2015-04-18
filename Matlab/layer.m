classdef layer < handle
    % This class defines Leabra 'layer' objects
    properties
        units       % an array with all the unit objects of the layer
        pct_act_scale  % The scaling factor for the outputs coming out from
                       % THIS layer. Notice this is different from C++
                       % version. Only updated with updt_long_avgs.               
        acts_p_avg  % a time-averaged version of acts_avg. It is updated only
                    % at the end of plus phases, so it is not a proper time
                    % average. See updt_long_avgs.
        netin_avg   % average net input for all units during last cycle               
        wt      % An NxI weights matrix, where the n-th row has the current
                % wt values for all inputs coming to the n-th unit
        ce_wt   % contrast-enhanced version of wt. ce_wt = SIG(wt).
        N       % number of units        
        fbi     % feedback inhibition
    end
    properties (Constant)
        ff = 1;     % gain for feedforward inhibition
        ff0 = 0.1;  % threshold for feedforward inhibition   
        fb = 0.5;   % gain for feedback inhibition
        fb_dt = 1/1.4; % time step for fb inhibition (fb_tau=1.4)
        gi = 2;     % overall gain on inhibition
        avg_act_dt = 0.01; % time step constant for updating acts_p_avg
    end
    properties (Dependent)
        acts_avg    % average activity of all units 
    end
            
    methods
        function lay = layer(dims)
        %% constructor for the layer class.
        % dims = dimensions of the layer: [rows,columns]        
        % Notice that wt must be initialized by the network.
        
        if length(dims) > 1
            lay.N = dims(1)*dims(2);
            lay.units = unit.empty;  % so I can make the assignment below
            lay.units(lay.N,1) = unit; % creating all units
            % notice how the unit array is 1-D. The 2-D structure of the
            % layer doesn't have meaning in this part of the code
        else
            error('dims argument should be of the type [rows,columns]');
        end                
        
        lay.acts_p_avg = lay.acts_avg;
        avg_act_n = max(sum(lay.activities > 0.4), 1);
        lay.pct_act_scale = 1/(avg_act_n + 2);
        lay.netin_avg = 0;
        lay.fbi = lay.fb*lay.acts_avg;
        
        end % end of constructor
        
        function acts = activities(obj)
        %% returns a vector with the activities of all units
            acts = zeros(obj.N,1); % is this still needed?
            acts(:) = [obj.units.act]';
        end
        
        function out = scaled_acts(obj)
            %% returns a vector with the scaled activities of all units
            out = zeros(obj.N,1);
            out(:) = [obj.units.act]';
            out = obj.pct_act_scale*out;
        end
        
        function cycle(lay,raw_inputs,ext_inputs)
            %% this function performs one Leabra cycle for the layer
            %raw_inputs = An Ix1 matrix, where I is the total number of inputs
            %             from all layers. Each input has already been scaled
            %             by the pct_act_scale of its layer of origin and by
            %             the wt_scale_rel factor.
            %ext_inputs = An Nx1 matrix denoting inputs that don't come
            %             from another layer, where N is the number of
            %             units in this layer. An empty matrix indicates
            %             that there are no external inputs.
             
            %% obtaining the net inputs            
            %netins = lay.wt*raw_inputs;
            netins = lay.ce_wt*raw_inputs;  % you use contrast-enhanced weights
            if ~isempty(ext_inputs)
                netins = netins + ext_inputs;
            end
            
            %% obtaining inhibition
            lay.netin_avg = mean(netins); 
            ffi = lay.ff * max(lay.netin_avg - lay.ff0, 0);
            lay.fbi = lay.fbi + lay.fb_dt * (lay.fb * lay.acts_avg - lay.fbi);
            gc_i = lay.gi * (ffi + lay.fbi); 
            
            %% calling the cycle method for all units
            for u = 1:lay.N  % a parfor here?
                lay.units(u).cycle(netins(u),gc_i);
            end                    
            
        end
        
        function clamped_cycle(lay,input)
            % sets all unit activities equal to the input and updates all
            % the variables as in the cycle function.
            % input = vector specifying the activities of all units
            for i = 1:lay.N  % parfor ?
                lay.units(i).clamped_cycle(input(i));
            end
            
            % updating inhibition for the next cycle
            lay.fbi = lay.fb*lay.acts_avg;             
        end
        
        function [avg_s,avg_m,avg_l] = averages(this)
            % Returns the s,m,l averages in the layer as vectors.
            % Notice that the ss average is not returned, and avg_l is not
            % updated before being returned.
            
            avg_s = [this.units.avg_s];  
            avg_m = [this.units.avg_m];
            avg_l = [this.units.avg_l];
        end
        
        function l_avg_rel = rel_avg_l(this)
            % Returns the relative values of avg_l. These are the dependent
            % variables rel_avg_l in all units used in latest XCAL
            l_avg_rel = [this.units.rel_avg_l];
        end
        
        function updt_avg_l(lay)
            % updates the long-term average (avg_l) of all the units in the
            % layer. Usually done after a plus phase.
            for i = 1:lay.N  % parfor?
                lay.units(i).updt_avg_l;
            end            
        end
        
        function updt_long_avgs(obj)
            % updates the acts_p_avg and pct_act_scale variables.
            % These variables update at the end of plus phases instead of
            % cycle by cycle. 
            % This version assumes full connectivity when updating
            % pct_act_scale. If partial connectivity were to be used, this
            % should have the calculation in WtScaleSpec::SLayActScale, in
            % LeabraConSpec.cpp 
            obj.acts_p_avg = obj.acts_p_avg + ...
                             obj.avg_act_dt * (obj.acts_avg - obj.acts_p_avg);
                         
            r_avg_act_n = max(round(obj.acts_p_avg * obj.N), 1);
            obj.pct_act_scale = 1/(r_avg_act_n + 2);                            
        end
        
        function reset(lay)
            % This function sets the activity of all units to random values, 
            % and all other dynamic variables are also set accordingly.            
            % Used to begin trials from a random stationary point.
            % The activity values may also be set to zero (see unit.reset)
            for u = 1:lay.N
                lay.units(u).reset;
            end
        end
        
        function avg_acts = get.acts_avg(lay)
            % get the value of acts_avg, the mean of unit activities
            avg_acts = mean(lay.activities);
        end
        
    end % end of methods
end