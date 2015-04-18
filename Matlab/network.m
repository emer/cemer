classdef network < handle
    % This class defines Leabra 'network' objects
    
    % At this point, the net input is being scaled assuming that if two
    % layers are connected, there is full connectivity.
    
    properties
        layers  % The layers in the network
        connections  % If layer j sends projections to layer i, then 
                     % connections(i,j) = c > 0; 0 otherwise. c specifies
                     % the relative strength of that connection with
                     % respect to the other projections to layer i.
        n_lays  % number of layers (number of objects in 'layers')
        n_units % total number of units in the network
        lrate   % learning rate for XCAL
    end
    
    properties (Constant)
        avg_l_lrn_max = 0.01; % max amount of "BCM" learning in XCAL
        avg_l_lrn_min = 0.0;  % min amount of "BCM" learning in XCAL
        m_in_s = 0.1; % proportion of medium to short term avgs. in XCAL
        m_lrn = 1;  % proportion of error-driven learning in XCAL
        d_thr = 0.0001; % threshold for XCAL "check mark" function
        d_rev = 0.1;    % reversal value for XCAL "check mark" function
        off = 1;    % 'offset' in the SIG function for contrast enhancement
        gain = 6;   % gain in the SIG function for contrast enhancement
    end
    
    properties (Dependent)
        m1; % the slope in the left part of XCAL's "check mark" 
    end
    
    methods
        function net = network(dim_layers,connections,w0)
        % constructor to the network class.
        % dim_layers = 1-D cell array. dim_layers{i} is a vector [j,k], 
        %              where j is the number of rows in layer i, and k is
        %              the number of columns in layer i.
        % connections = a 2D array. If layer j sends projections to 
        %               layer i, then connections(i,j) = c > 0; 0 otherwise
        % w0 = w0 is a cell array. w0{i,j} is the weight matrix with the
        %      initial weights for the connections from layer j to i             
        
        %% Initial test of argument dimensions
        n_lay = length(dim_layers);  % number of layers
        [nrc, ncc] = size(connections);
        if nrc ~= ncc
            error('Non-square connections matrix in network constructor');
        end
        if nrc ~= n_lay
            error('Number of layers inconsistent with connection matrix in network constructor');
        end
        if sum(size(w0) == size(connections)) < 2
            error('Inconsistent dimensions between initial weights and connectivity specification in network constructor');
        end   
        if min(min(connections)) < 0
            error('Negative projection strengths between layers are not allowed in connections matrix');
        end
        net.n_lays = n_lay;
        
        %% Normalizing the rows of 'connections' so they add to 1
        row_sums = sum(connections,2);
        for row = 1:n_lay
            if row_sums(row) > 0
                connections(row,:) = bsxfun(@rdivide,connections(row,:),row_sums(row));
            end
        end
        
        %% Creating layers
        net.layers = cell(1,n_lay);    % cell array that will contain layers 
        net.n_units = 0;   % counting number of units in the network
        for i = 1:n_lay
            net.layers{i} = layer(dim_layers{i});
            net.n_units = net.n_units + net.layers{i}.N;
        end              
       
        %% Second test of argument dimensions
        for i = 1:n_lay
            for j = 1:n_lay
                if isempty(w0{i,j})
                    if connections(i,j) > 0
                        error('Connected layers have no connection weights');
                    end
                else
                    if connections(i,j) == 0
                        error('Non-empty weight matrix for unconnected layers');
                    end
                    [r,c] = size(w0{i,j});
                    if net.layers{i}.N ~= r || net.layers{j}.N ~= c
                        error('Initial weigths are inconsistent with layer dimensions');
                    end
                end
            end
        end
        net.connections = connections; % assigning layer connection matrix
        
        %% Setting the inital weights for each layer
        % first find how many units project to the layer in all the network
        lay_inp_size = zeros(1,n_lay);
        for i = 1:n_lay
            for j= 1:n_lay
                if connections(i,j) > 0  % if layer j projects to layer i
                    lay_inp_size(i) = lay_inp_size(i) + net.layers{j}.N;
                end
            end
        end
        
        % add the weights for each entry in w0 as a group of columns in wt
        for i = 1:n_lay
            net.layers{i}.wt = zeros(net.layers{i}.N,lay_inp_size(i));
            index = 1;
            for j = 1:n_lay
                if connections(i,j) > 0  % if layer j projects to layer i
                    nj = net.layers{j}.N;
                    net.layers{i}.wt(:,index:index+nj-1) = w0{i,j};
                    index = index + nj;
                end
            end
        end 
        
        % set the contrast-enhanced version of the weights
        for lay = 1:n_lay
            net.layers{lay}.ce_wt = bsxfun(@rdivide, 1, 1 + ...
                (net.off*(1 - net.layers{lay}.wt)./net.layers{lay}.wt).^net.gain);
        end
        
        % set default learning rate
        net.lrate = 0.1;
        
        end  % end of constructor
        %%
        function cycle(obj,inputs,clamp_inp)
            % this function calls the cycle method for all layers.
            % inputs = a cell array. inputs{i} is  a matrix that for layer i
            %          specifies the external input to each of its units.
            %          An empty matrix denotes no input to that layer.
            % clamp_inp = a binary flag. 1 -> layers are clamped to their
            %             input value. 0 -> inputs summed to netins.
            
            %% Testing the arguments and reshaping the input
            if ~iscell(inputs)
                error('First argument to cycle function should be an inputs cell');
            end
            if length(inputs) ~= obj.n_lays
                error('Number of layers inconsistent with number of inputs in network cycle');
            end
            for inp = 1:obj.n_lays  % reshaping inputs into column vectors
                if ~isempty(inputs{inp})
                    inputs{inp} = reshape(inputs{inp},obj.layers{inp}.N,1);
                end
            end
            
            %% First we set all clamped layers to their input values
            clamped_lays = zeros(1,obj.n_lays);
            if clamp_inp
                for lay = 1:obj.n_lays
                    if ~isempty(inputs{lay})
                        obj.layers{lay}.clamped_cycle(inputs{lay});
                        clamped_lays(lay) = 1;
                    end
                end
            end
            
            %% We make a copy of the scaled activity for all layers
            scaled_acts = cell(1,obj.n_lays);
            for lay = 1:obj.n_lays
                scaled_acts{lay} = obj.layers{lay}.scaled_acts;
            end
            
            %% For each unclamped layer, we put all its scaled inputs in one
            %  column vector, and call its cycle function with that vector
            for recv = 1:obj.n_lays
                if ~clamped_lays(recv) % if the layer is not clamped
                    % for each 'recv' layer we find its input vector
                    long_input = zeros(obj.n_units,1); % preallocating for speed
                    n_inps = 0;  % will have the # of input units to 'recv'
                    n_sends = 0; % will have the # of layers sending to 'recv'
                    wt_scale_rel = obj.connections(recv,logical(obj.connections(recv,:)));
                    % wt_scale_rel is the non-zero entires of row 'recv'
                    for send = 1:obj.n_lays
                        if obj.connections(recv,send) > 0
                            n_sends = n_sends + 1;
                            
                            long_input(1+n_inps:n_inps+obj.layers{send}.N) = ...
                                wt_scale_rel(n_sends) * scaled_acts{send};
                            
                            n_inps = n_inps + obj.layers{send}.N;
                        end
                    end
                    % now we call 'cycle'
                    obj.layers{recv}.cycle(long_input(1:n_inps),inputs{recv});                   
                end
            end
            
        end % end of 'cycle' function
        
        function XCAL_learn(this)
            % XCAL_learn() applies the XCAL learning equations in order to
            % modify the weights in the network. This is typically done at
            % the end of a plus phase. The equations used come from:
            % https://grey.colorado.edu/ccnlab/index.php/Leabra_Hog_Prob_Fix#Adaptive_Contrast_Impl
            % Soft weight bounding and contrast enhancememnt are as in:
            % https://grey.colorado.edu/emergent/index.php/Leabra
            
            %% updating the long-term averages
            for lay = 1:this.n_lays
               this.layers{lay}.updt_avg_l; 
            end
            
            %% Extracting the averages for all layers            
            avg_s = cell(1,this.n_lays);
            avg_m = cell(1,this.n_lays);
            avg_l = cell(1,this.n_lays);
            avg_s_eff = cell(1,this.n_lays);
            for lay = 1:this.n_lays
                [avg_s{lay},avg_m{lay},avg_l{lay}] = this.layers{lay}.averages;
                avg_s_eff{lay} = this.m_in_s*avg_m{lay} + (1-this.m_in_s)*avg_s{lay};
                % at this point, the avg_X vectors are row vectors
            end
            
            %% obtaining avg_l_lrn
            avg_l_lrn = cell(1,this.n_lays);
            for lay = 1:this.n_lays
                avg_l_lrn{lay} = this.avg_l_lrn_min + this.layers{lay}.rel_avg_l * ...
                                 (this.avg_l_lrn_max - this.avg_l_lrn_min);                
            end
            %assignin('base','avg_l_lrn',avg_l_lrn) % if you wanna see                  
            
            %% For each connection matrix, calculate the intermediate vars.
            srs = cell(this.n_lays); % srs{i,j} = matrix of short-term averages
                                     % where the rows correspond to the
                                     % units of the receiving layer, columns
                                     % to units of the sending layer.
            srm = cell(this.n_lays); % ditto for avg_m

            for rcv = 1:this.n_lays
                for snd = rcv:this.n_lays
                    % notice we only calculate the 'upper triangle' of the
                    % cell arrays because of symmetry
                    if this.connections(rcv,snd)>0 || this.connections(snd,rcv)>0
                       srs{rcv,snd} = (avg_s_eff{rcv}') * avg_s_eff{snd};
                       srm{rcv,snd} = (avg_m{rcv}') * avg_m{snd};

                       if snd ~= rcv % using symmetry
                            srs{snd,rcv} = srs{rcv,snd}';
                            srm{snd,rcv} = srm{rcv,snd}';                             
                       end
                    end
                end
            end
            
            %% calculate the weight changes
            dwt = cell(this.n_lays); % dwt{i,j} is the matrix of weight changes
                                     % for the weights from layer j to i
            for rcv = 1:this.n_lays
                for snd = 1:this.n_lays
                    if this.connections(rcv,snd) > 0
                        sndN = this.layers{snd}.N;
                        dwt{rcv,snd} = this.lrate * ...
                                   ( this.m_lrn * this.xcal(srs{rcv,snd},srm{rcv,snd}) + ...
                        bsxfun(@times,avg_l_lrn{rcv}', this.xcal(srs{rcv,snd},repmat(avg_l{rcv}',1,sndN))) );
                    end
                end
            end
            
            %% update weights (with weight bounding)
            for rcv = 1:this.n_lays                
                DW = []; % maybe I should preallocate...
                for snd = 1:this.n_lays
                    if this.connections(rcv,snd) > 0
                        DW = [DW, dwt{rcv,snd}];
                    end
                end
                if ~isempty(DW)
                    % Here's the weight bounding part, as in the CCN book
                    idxp = logical(this.layers{rcv}.wt > 0);
                    idxn = ~idxp;
                    
                    this.layers{rcv}.wt(idxp) = this.layers{rcv}.wt(idxp) + ...
                                            (1 - this.layers{rcv}.wt(idxp)) .* DW(idxp);
                    this.layers{rcv}.wt(idxn) = this.layers{rcv}.wt(idxn) + ...
                                            this.layers{rcv}.wt(idxn) .* DW(idxn);
                    %disp(['mean weight change: ' num2str(sum(sum(abs(DW)))/numel(DW))]);
                end
            end
            
            %% set the contrast-enhanced version of the weights
            for lay = 1:this.n_lays
                this.layers{lay}.ce_wt = bsxfun(@rdivide, 1, 1 + ...
                    (this.off*(1 - this.layers{lay}.wt)./this.layers{lay}.wt).^this.gain);
            end
        end % end XCAL_learn method 
        
        function updt_long_avgs(this)
            % updates the acts_p_avg and pct_act_scale variables for all layers. 
            % These variables update at the end of plus phases instead of
            % cycle by cycle. The avg_l values are not updated here.
            % This version assumes full connectivity when updating
            % pct_act_scale. If partial connectivity were to be used, this
            % should have the calculation in WtScaleSpec::SLayActScale, in
            % LeabraConSpec.cpp 
            for lay = 1:this.n_lays
                this.layers{lay}.updt_long_avgs;              
            end
        end
        
        function m = get.m1(this)
            % obtains the m1 factor: the slope of the left-hand line in the
            % "check mark" XCAL function. Notice it includes the negative
            % sign.
            m = (this.d_rev - 1)/this.d_rev;
        end
                
        function f = xcal(this, x, th)
            % this function implements the "check mark" function in XCAL.
            % x = an array of abscissa values.
            % th = an array of threshold values, same size as x
            f = zeros(size(x));
            idx1 = logical((x > this.d_thr) & (x < this.d_rev*th));
            idx2 = logical(x >= this.d_rev*th);
            f(idx1) = this.m1 * x(idx1);
            f(idx2) = x(idx2) - th(idx2);            
        end
        
        function reset(this)
            % This function sets the activity of all units to random values, 
            % and all other dynamic variables are also set accordingly.            
            % Used to begin trials from a random stationary point.
            for lay = 1:this.n_lays
                this.layers{lay}.reset;
            end
        end
        
        function set_weights(net,w)
            % This function receives a cell array w, which is like the cell
            % array w0 in the constructor: w{i,j} is the weight matrix with
            % the initial weights for the connections from layer j to layer
            % i. The weights are set to the values of w.
            % This whole function is a slightly modified copypasta of the
            % constructor.
            
            %% First we test the dimensions of w
            if sum(size(w) == size(net.connections)) < 2
                error('Inconsistent dimensions between weights and connectivity specification in set_weights');
            end
            for i = 1:net.n_lays
                for j = 1:net.n_lays
                    if isempty(w{i,j})
                        if net.connections(i,j) > 0
                            error('Connected layers have no connection weights');
                        end
                    else
                        if net.connections(i,j) == 0
                            error('Non-empty weight matrix for unconnected layers');
                        end
                        [r,c] = size(w{i,j});
                        if net.layers{i}.N ~= r || net.layers{j}.N ~= c
                            error('Initial weigths are inconsistent with layer dimensions');
                        end
                    end
                end
            end
            
            %% Now we set the weights
            % first find how many units project to the layer in all the network
            lay_inp_size = zeros(1,net.n_lays);
            for i = 1:net.n_lays
                for j= 1:net.n_lays
                    if net.connections(i,j) > 0  % if layer j projects to layer i
                        lay_inp_size(i) = lay_inp_size(i) + net.layers{j}.N;
                    end
                end
            end
            
            % add the weights for each entry in w0 as a group of columns in wt
            for i = 1:net.n_lays
                net.layers{i}.wt = zeros(net.layers{i}.N,lay_inp_size(i));
                index = 1;
                for j = 1:net.n_lays
                    if net.connections(i,j) > 0  % if layer j projects to layer i
                        nj = net.layers{j}.N;
                        net.layers{i}.wt(:,index:index+nj-1) = w{i,j};
                        index = index + nj;
                    end
                end
            end   
            
            % set the contrast-enhanced version of the weights
            for lay = 1:net.n_lays
                net.layers{lay}.ce_wt = bsxfun(@rdivide, 1, 1 + ...
                    (net.off*(1 - net.layers{lay}.wt)./net.layers{lay}.wt).^net.gain);
            end
        end
        
        function w = get_weights(net)
            % This function returns a 2D cell array w.
            % w{rcv,snd} contains the weight matrix for the projections from
            % layer snd to layer rcv.
            w = cell(net.n_lays);
            for rcv = 1:net.n_lays
                idx1 = 1; % column where the weights from layer 'snd' start
                for snd = 1:net.n_lays
                    if net.connections(rcv,snd) > 0
                        Nsnd = net.layers{rcv}.N;
                        w{rcv,snd} = net.layers{rcv}.wt(:,idx1:idx1+Nsnd-1);
                        idx1 = idx1 + Nsnd;
                    end
                end               
            end
        end
        
    end % end of methods

end