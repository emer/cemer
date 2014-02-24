*** Settings ***

Library           Process
Library           OperatingSystem

*** Keywords ***

#
# Top-tier keywords (called from Suite)

Run Default Test
    [Arguments]               ${project}    ${test-dir}
    Set Variables             ${project}    60 secs    ${test-dir}
    Set Metrics               avg  0.15  0.25
    Create CSS Default        ${test-dir}
    Run Model
    Check Preferred
    Check Required

Set Variables
    [Arguments]         ${namearg}         ${time}           ${test-dir}
    Set Test Variable   ${name}            ${namearg}
    Set Test Variable   ${project-file}    ${name}.proj
    Set Test Variable   ${script-file}     ${test-dir}/${name}.css
    Set Test Variable   ${timeout}         ${time}
    Set Test Variable   ${baseline-file}   ${test-dir}/${name}.baseline.json
    Set Test Variable   ${record-file}     ${test-dir}/${name}.record.json

Set Metrics
    [Arguments]         @{metrics}
    @{metric-list} =    Create List        @{metrics}
    Set Test Variable   @{metric-list}

Run Model
    ${output} =                                     Run Emergent     ${project-file}   ${script-file}   ${timeout}
    ${actuals} =                                    Get Results      ${output}
    Append To File                                  ${record-file}   ${actuals}\n  
    ${baseline} =                                   Get Baseline     ${baseline-file}
    Run Keyword If      '${baseline}' == 'FAIL'     Add To Baseline  ${baseline-file}  ${actuals}
    Run Keyword If      '${baseline}' == 'FAIL'     Set Variable     ${baseline}       ${actuals}
    Set Test Variable   ${baseline}
    Set Test Variable   ${actuals}

Check Preferred
    :FOR  ${metric}  ${preferred}  ${required}  IN  @{metric-list}
    \     Run Keyword And Continue On Failure   Check Metric     ${metric}  ${preferred}  ${actuals}  ${baseline}

Check Required
    :FOR  ${metric}  ${preferred}  ${required}  IN  @{metric-list}
    \     Run Keyword And Continue On Failure   Check Metric     ${metric}  ${required}   ${actuals}  ${baseline}


#
#  Second tier keywords

Run Emergent
    [Arguments]          ${project}   ${script}    ${timeout}
    Start Process        emergent -nogui -ni -p ${project} -s ${script}   shell=True  # Doesn't seem to work without shell - misses args
    ${result} =          Wait For Process          timeout=${timeout}  
    Should Be Equal As Integers                    ${result.rc}  0
    [Return]             ${result.stdout}${result.stderr}

Check Metric
    [Arguments]          ${metric}    ${fraction}  ${actuals}  ${baseline}
    ${value} =           Get Value From JSON       ${metric}   ${actuals}
    ${target} =          Get Value From JSON       ${metric}   ${baseline}
    In Range Fraction    ${metric}    ${value}     ${target}   ${fraction}

#
# JSON handlers

Get Results
    [Arguments]          ${output}
    ${result} =          Should Match Regexp       ${output}   {.*}     msg=No results found in output for ${name}: ${output}     values=False
    [Return]             ${result}

Get Baseline             
    [Arguments]          ${file}
    ${status}            ${content} =     Run Keyword And Ignore Error       Get File     ${file}
    ${content} =         Set Variable If    '${status}' == 'FAIL'   ${EMPTY}     ${content}
    ${status}            ${baseline} =    Run Keyword And Ignore Error       Should Match Regexp       ${content}   {[^\"]*\"name\"[^:]*:[^\"]*\"${name}\"[^}]*}
    ${baseline} =        Set Variable If    '${status}' == 'FAIL'   ${status}     ${baseline}
    [Return]             ${baseline}

Add To Baseline
    [Arguments]          ${filename}  ${actuals}
    Append To File       ${baseline-file}   ${actuals}  
    Log                  No baseline for project "${name}".    WARN
    Pass Execution       Created baseline from results of this run.

Get Value From JSON
    [Arguments]          ${field}     ${json}
    ${match}             ${value} = 
    ...                  Should Match Regexp       ${json}  (?i)\"${field}\"[^:]*:\\s*(\\d+\\.?\\d*)
    [Return]             ${value}


#
# Testing values in ranges

In Range Fraction
    [Arguments]          ${name}    ${value}   ${target}   ${fraction}
    ${low} =             Evaluate   ((1-${fraction})*${target})
    ${high} =            Evaluate   ((1+${fraction})*${target})
    In Range             ${name}    ${value}   ${low}   ${high}

In Range
    [Arguments]          ${name}    ${value}   ${low}   ${high}
    Greater Or Equal     ${name}    ${value}   ${low}
    Less Or Equal        ${name}    ${value}   ${high}
    
Greater Or Equal
    [Arguments]          ${name}    ${left}    ${right}
    ${isGreater} =       Evaluate              (${left} >= ${right})
    Should Be True       ${isGreater}          ${name} = ${left} is LOW (${right})

Less Or Equal
    [Arguments]          ${name}    ${left}   ${right}
    ${isLess} =          Evaluate             (${left} <= ${right})
    Should Be True       ${isLess}            ${name} = ${left} is HIGH (${right})

#
# Creating CSS files

Create CSS Default
    [Arguments]         ${test-dir}
    Create CSS Custom                               ${script-file}   ${test-dir}
    Add CSS Standard Randomize                      ${script-file}   ${name}
    Add CSS Standard RunProgram                     ${script-file}   ${name}    LeabraBatch    Network_0    StdInputData
    Add CSS Standard Output                         ${script-file}   ${name}    OutputEpochsToTrain   EpochOutputData

Create CSS Standard
    [Arguments]         ${script}     ${name}
    Create File         ${script}     Program *program = .projects["${name}"].programs.gp["Tests"]["Test"];\nprogram->Run();\n

Create CSS Custom
    [Arguments]         ${script}     ${test-dir}
    Create File         ${script}     \#include "${test-dir}/leabra.css"\n\n

Add CSS Standard Randomize
    [Arguments]         ${script}     ${project}
    Append To File      ${script}     SetRandom(\n
    Append To File      ${script}     "${project}",\n
    Append To File      ${script}     "LeabraAll_Std",\n
    Append To File      ${script}     "LeabraTrain",\n
    Append To File      ${script}     "rnd_init");\n

Add CSS Standard RunProgram
    [Arguments]         ${script}     ${project}    ${program}   ${network}  ${data}
    Append To File      ${script}     RunProgram(\n
    Append To File      ${script}     "${project}",\n
    Append To File      ${script}     "LeabraAll_Std",\n
    Append To File      ${script}     "${program}",\n
    Append To File      ${script}     "${network}",\n
    Append To File      ${script}     "InputData",\n
    Append To File      ${script}     "${data}");\n

Add CSS Standard Output
    [Arguments]         ${script}     ${project}    ${method}    ${table}
    Append To File      ${script}     ${method}(\n
    Append To File      ${script}     "${project}",\n
    Append To File      ${script}     "${project}",\n
    Append To File      ${script}     "OutputData",\n
    Append To File      ${script}     "${table}");\n
    
Add CSS Verify Standard
    [Arguments]         ${script}     ${project}
    Append To File      ${script}     VerifyStandard("${project}");\n