/**
 * emergent.txt (aka emergent.txt.cpy)
 * Copyright (c) 2014 eCortex, Inc.
 * 
 * This file is part of the Emergent Test Framework.
 *
 * The Emergent Test Framework is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * The Emergent Test Framework is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the Emergent Test Framework.  If not, see http://www.gnu.org/licenses/.
 *
 * Emergent Test Framework
 * Version 0.8
 * Compatible with Emergent 6.x
 * 
 */

*** Settings ***

Library           Process
Library           OperatingSystem
Library           Collections

*** Keywords ***

#
# Top-tier keywords (called from Test)

Initialize Standard Test
    [Arguments]               ${project}    ${test-dir}
    Set Variables             ${project}    ${test-dir}
   
Run Standard Test
    Run Model
    Check Preferred
    Check Required

Set Timeout
    [Arguments]         ${time}
    Set Test Variable   ${timeout}         ${time}

Add Metric
    [Arguments]         @{metric}
    Append To List      ${metric-list}     @{metric}

#
# Level two keywords - called from top tier

Set Variables
    [Arguments]         ${namearg}         ${test-dir}
    Set Test Variable   ${S}               ${SPACE * 4}
    Set Test Variable   ${name}            ${namearg}
    Set Test Variable   ${project-file}    ${name}.proj
    Set Test Variable   ${script-file}     ${test-dir}/${name}.css.txt
    Set Test Variable   ${baseline-file}   ${test-dir}/${name}.baseline.json
    Set Test Variable   ${record-file}     ${test-dir}/${name}.record.json
    @{metric-list} =    Create List
    Set Test Variable   ${metric-list}

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

