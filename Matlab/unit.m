classdef unit < handle
    % This class defines Leabra unit objects.
    
    % unit is a handle class so that its methods and those of layer objects
    % can modify the properties of their units. For example, if it weren't
    % a handle class then 'lay' in the method clamped_cycle(lay,input) of
    % the layer class would be a layer object with indepenent copies of the
    % unit objects.
    properties
        act     % "firing rate" of the unit
        avg_ss  % super short-term average of act
        avg_s   % short-term average of act
        avg_m   % medium-term average of act
        avg_l   % long-term average of act
        net     % net input. Asymptotically approaches net_raw (see cycle).
        v_m     % membrane potential
        vm_eq   % a version of v_m that doesn't reset with spikes
        adapt   % adaptation, as in the AdEx model        
        spike   % a flag that indicates spiking threshold was crossed
    end
    properties (Constant) % the same for all units
        net_dt = 1/1.4; % time step constant for update of 'net'
        integ_dt = 1;   % time step constant for integration of cycle dynamics
        vm_dt = 1/3.3;  % time step constant for membrane potential
        l_dn_dt = 1/2.5; % time step constant for avg_l decrease
        adapt_dt = 1/144; % time step constant for adaptation
        ss_dt = 0.5;    % time step for super-short average
        s_dt = 0.5;     % time step for short average
        m_dt = 0.1;     % time step for medium-term average
        avg_l_dt = 0.1; % time step for long-term average
        avg_l_max = 1.5; % max value of avg_l
        avg_l_min = 0.1; % min value of avg_l
        e_rev_e = 1;    % excitatory reversal potential
        e_rev_i = .25;  % inhibitory reversal potential
        e_rev_l = 0.3;  % leak reversal potential
        gc_l = 0.1;     % leak conductance
        thr = 0.5;      % normalized "rate threshold"
        spk_thr = 1.2;  % normalized spike threshold
        vm_r = 0.3;     % reset membrane potential after spike
        vm_gain = 0.04; % gain that voltage produces on adaptation
        spike_gain = 0.00805; % effect of spikes on adaptation
        l_up_inc = 0.2; % increase in avg_l if avg_m has been 'large'
    end
    
    properties (Dependent)
        rel_avg_l; % value of avg_l relative to its max and min (for XCAL)
    end
           
    methods
        function u = unit(varargin)
            % constructor for the unit class. You can either call it with no
            % arguments, or specify the value of any property with syntax:
            % unit('PropertyName', PropertyValue, ...)
            % Only dynamic properties can be set with the constructor.
            
            % Setting initial values for all dynamic properties            
            u.net = 0;
            u.v_m = 0.3;
            u.vm_eq = 0.3;
            u.adapt = 0;
            u.act = .2;
            u.spike = 0;
            u.avg_ss = u.act;
            u.avg_s = u.act;
            u.avg_m = u.act;
            u.avg_l = u.avg_l_min;
            
            if nargin > 0  % if the constructor was called with arguments
                if mod(nargin,2) ~= 0
                    error('unit constructor must receive an even number or arguments');
                end
                
                for arg = 1:2:nargin
                    if ~ischar(varargin{arg})
                        error('Call to unit constructor must specify a property string and a numeric value');
                    end
                    switch varargin{arg}
                        case 'avg_ss'
                            u.avg_ss = varargin{arg+1};
                        case 'avg_s'
                            u.avg_s = varargin{arg+1};
                        case 'avg_m'
                            u.avg_m = varargin{arg+1};
                        case  'avg_l'
                            u.avg_l = varargin{arg+1};
                        case 'net'
                            u.net = varargin{arg+1};
                        case 'v_m'
                            u.v_m = varargin{arg+1};
                        case 'vm_eq'
                            u.vm_eq = varargin{arg+1};
                        case 'adapt'
                            u.adapt = varargin{arg+1};
                        case 'act'
                            u.act = varargin{arg+1};
                        otherwise
                            error(['Unknown property ',varargin{arg},' in unit constructor']);
                    end
                end
            end
        end % end of class constructor
        
        function cycle(u,net_raw,gc_i)
            % Does one Leabra cycle. Called by the layer cycle method.
            % net_raw = instantaneous, scaled, received input
            % gc_i = fffb inhibition
            
            %% updating net input
            u.net = u.net + u.integ_dt * u.net_dt * (net_raw - u.net);
            
            %% Finding membrane potential
            I_net = u.net*(u.e_rev_e - u.v_m) + u.gc_l*(u.e_rev_l - u.v_m) + ...
                    gc_i*(u.e_rev_i - u.v_m);
            % almost half-step method for updating v_m (adapt doesn't half step)
            v_m_h = u.v_m + 0.5*u.integ_dt*u.vm_dt*(I_net - u.adapt);
            I_net_h = u.net*(u.e_rev_e - v_m_h) + u.gc_l*(u.e_rev_l - v_m_h) + ...
                    gc_i*(u.e_rev_i - v_m_h);
            u.v_m = u.v_m + u.integ_dt*u.vm_dt*(I_net_h - u.adapt);
            u.vm_eq = u.vm_eq + u.integ_dt*u.vm_dt*(I_net_h - u.adapt);
            
            %% Finding activation
            % finding threshold excitatory conductance
            g_e_thr = (gc_i*(u.e_rev_i-u.thr) + u.gc_l*(u.e_rev_l-u.thr) ...
                      - u.adapt) / (u.thr - u.e_rev_e);
            % finding whether there's an action potential
            if u.v_m > u.spk_thr
                u.spike = 1;
                u.v_m = u.vm_r;
            else
                u.spike = 0;
            end
            % finding instantaneous rate due to input
            if u.vm_eq <= u.thr
                new_act = unit.nxx1(u.vm_eq - u.thr);
            else
                new_act = unit.nxx1(u.net - g_e_thr);
            end
            % update activity
            u.act = u.act + u.integ_dt*u.vm_dt*(new_act - u.act);
            
            %% Updating adaptation
            u.adapt = u.adapt + u.integ_dt*(u.adapt_dt*(u.vm_gain*(u.v_m - u.e_rev_l) ...
                      - u.adapt) + u.spike*u.spike_gain);
                  
            %% updating averages
            u.avg_ss = u.avg_ss + u.integ_dt * u.ss_dt * (u.act - u.avg_ss);
            u.avg_s = u.avg_s + u.integ_dt * u.s_dt * (u.avg_ss - u.avg_s);
            u.avg_m = u.avg_m + u.integ_dt * u.m_dt * (u.avg_s - u.avg_m);
            
        end  % end of cycle function
        
        function clamped_cycle(u,input)
            % This function performs one cycle of the unit when its activty
            % is clamped to an input value. The activity is set to be equal
            % to the input, and all the averages are updated accordingly.
            
            %% Clamping the activty to the input
            u.act = input;
            
            %% updating averages
            u.avg_ss = u.avg_ss + u.integ_dt * u.ss_dt * (u.act - u.avg_ss);
            u.avg_s = u.avg_s + u.integ_dt * u.s_dt * (u.avg_ss - u.avg_s);
            u.avg_m = u.avg_m + u.integ_dt * u.m_dt * (u.avg_s - u.avg_m);
        end
        
        function updt_avg_l(u)
            % This fuction updates the long-term average 'avg_l' 
            % u = this unit
            % Based on the description in:
            % https://grey.colorado.edu/ccnlab/index.php/Leabra_Hog_Prob_Fix#Adaptive_Contrast_Impl
            
            if u.avg_m > 0.2
                u.avg_l = u.avg_l + u.avg_l_dt*(u.avg_l_max - u.avg_m);
            else
                u.avg_l = u.avg_l + u.avg_l_dt*(u.avg_l_min - u.avg_m);
            end
        end
        
        function l_avg_rel = get.rel_avg_l(u)
            % obtain the value of rel_avg_l
            l_avg_rel = (u.avg_l - u.avg_l_min)/(u.avg_l_max - u.avg_l_min);
        end
        
        function reset(u)
            % This function sets the activity to a random value, and sets
            % all activity time averages equal to that value.
            % Used to begin trials from a random stationary point.
            %u.act = 0.05 + 0.9*rand;
            u.act = 0;  % Randy begins trials with act=0
            u.avg_ss = u.act;
            u.avg_s = u.act;
            u.avg_m = u.act;
            u.avg_l = u.act;
            u.net = 0;
            u.v_m = 0.3;
            u.vm_eq = 0.3;
            u.adapt = 0;            
            u.spike = 0;            
        end
    end  % end of non-static methods 
    
    methods (Static)
        function f = nxx1(points)
            % f = nxx1(points) calculates the noisy x/(x+1) function for
            % all values in the vector 'points'. The returned values come
            % from the convolution of x/(x+1) with a Gaussian function.
            % To avoid calculating the convolution every time, the first
            % time the function is called a vector 'nxoxp1' is created,
            % corresponding to the values of nxx1 at all the points in the
            % vector 'nxx1_dom'. Once these vectors are in the workspace,
            % subsequent calls to nxx1 use interpolation with these
            % vectors in order to calculate their return values.
            
            persistent nxoxp1 nxx1_dom
            
            if isempty(nxoxp1) || isempty(nxx1_dom)
            % we don't have precalculated vectors for interpolation
                n_points = 2000; % size of the precalculated vectors
                gain = 100; % gain of the xx1 function
                mid = 2; % mid length of the domain
                domain = linspace(-mid,mid,n_points); % will be 'nxx1_dom'
                dom_g = linspace(-2*mid,2*mid,2*n_points); % domain of Gaussian
                values = zeros(1,n_points); % will be 'nxoxp1'
                sd = .005; % standard deviation of the Gaussian
                gaussian = exp(-(dom_g.^2)/(2*sd^2))/(sd*sqrt(2*pi));
                xx1 = max(gain*domain,zeros(1,n_points))./(max(gain*domain,zeros(1,n_points))+1);
                
                for p = 1:n_points
                    low = n_points - p + 1;
                    high = 2*n_points - p;
                    values(p) = sum(xx1.*gaussian(low:high));
                    values(p) = values(p)/sum(gaussian(low:high));
                end
                nxx1_dom = domain;
                nxoxp1 = values;  
                %disp('First call to nxx1!');
            end
            f = interp1(nxx1_dom,nxoxp1,points,'nearest','extrap');
        end
               
    end  % end of static methods
end
        