R"***^***(
        
            //http://bl.ocks.org/mbostock/5577023
        
            Highcharts.setOptions({
                colors: ['#a6cee3','#1f78b4','#b2df8a','#33a02c','#fb9a99','#e31a1c',
                '#fdbf6f','#ff7f00','#cab2d6','#6a3d9a','#ffff99','#b15928']
            });
            
            $(function () {    
                 var kdeChart = new Highcharts.Chart({
                    chart: {
                        renderTo: 'kde',
                        zoomType: 'xy'
                    },
                    title: {
                        text: 'Kernel Density Estimate'
                    },
                    subtitle: {
                        text: '<a href="https://github.com/ctrychta/velox">generated by velox</a>'
                    },
                    xAxis: {
                        title: {
                            text: 'Time'
                        }
                    },
                    yAxis: {
                        title: {
                            text: 'Density (arb. unit)'
                        },
                        minPadding: 0,
                        maxPadding: 0
                    },
                    tooltip: {
                        crosshairs: true,
                        shared: true,
                    },
                    series: [{
                        type: 'spline',
                        name: 'Probability Density',
                        id: 'pdf',
                        marker: {
                            enabled:false
                        },
                        data: []
                    } , {
                        type: 'line',
                        name: 'Mean',
                        id: 'mean',
                        marker: {
                            enabled: false,
                            states: {
                                hover: {
                                    enabled: false
                                }
                            }
                        },
                        data: []
                    } , {
                        type: 'line',
                        name: 'Median',
                        id: 'median',
                        marker: {
                            enabled:false,
                            states: {
                                hover: {
                                    enabled: false
                                }
                            }
                        },
                        data: []
                    }]
                });
                
                var samplesChart = new Highcharts.Chart({
                    chart: {
                        renderTo: 'samples',
                        zoomType: 'xy'
                    },
                    title: {
                        text: 'Samples'
                    },
                    subtitle: {
                        text: '<a href="https://github.com/ctrychta/velox">generated by velox</a>'
                    },
                    yAxis: {
                        title: {
                            text: 'Time'
                        }
                    },
                    series: [{
                        type: 'scatter',
                        name: 'Sample',
                        id: 'sample',
                        marker: {
                            enabled:true
                        },
                        color: '#a6cee3',
                        data: []
                    } , {
                        type: 'line',
                        name: 'Q3 + 3 * IQR',
                        id: 'highSevere',
                        marker: {
                            enabled: false,
                            states: {
                                hover: {
                                    enabled: false
                                }
                            }
                        },
                        color: '#ff7f00',
                        data: []
                    } , {
                        type: 'line',
                        name: 'Q3 + 1.5 * IQR',
                        id: 'highMild',
                        marker: {
                            enabled: false,
                            states: {
                                hover: {
                                    enabled: false
                                }
                            }
                        },
                        color: '#fdbf6f',
                        data: []
                    } , {
                        type: 'line',
                        name: 'Q1 - 1.5 * IQR',
                        id: 'lowMild',
                        marker: {
                            enabled: false,
                            states: {
                                hover: {
                                    enabled: false
                                }
                            }
                        },
                        color: '#cab2d6',
                        data: []
                    } , {
                        type: 'line',
                        name: 'Q1 - 3 * IQR',
                        id: 'lowSevere',
                        marker: {
                            enabled: false,
                            states: {
                                hover: {
                                    enabled: false
                                }
                            }
                        },
                        color: '#6a3d9a',
                        data: []
                    }]
                });
            
                var rawMeasurementsChart = new Highcharts.Chart({
                    chart: {
                        renderTo: 'raw-measurements',
                        zoomType: 'xy'
                    },
                    title: {
                        text: 'Raw Measurements'
                    },
                    subtitle: {
                        text: '<a href="https://github.com/ctrychta/velox">generated by velox</a>'
                    },
                     xAxis: {
                        title: {
                            text: 'Iterations'
                        }
                    },
                    yAxis: {
                        title: {
                            text: 'Time'
                        },
                        minPadding: 0,
                        maxPadding: 0
                    },
                    series: [{
                        type: 'scatter',
                        name: 'Measurement',
                        id: 'measurement',
                        marker: {
                            enabled:true
                        },
                        color: '#a6cee3',
                        data: []
                    } , {
                        type: 'line',
                        name: 'Regression',
                        id: 'regression',
                        marker: {
                            enabled: false,
                            states: {
                                hover: {
                                    enabled: false
                                }
                            }
                        },
                        enableMouseTracking: false,
                        color: '#e31a1c',
                        data: []
                    }]
                });
                
                function updateData(id) {
                    $("#benchmarks .current").removeClass("current");
                    $('#' + id).parent().addClass("current");
  
                    var benchData = benchmarkData[id];
                    
                    $('#benchmark-name').text(benchData.name);

                    // Set sample summary
                    
                    var idToSummaryValue = { 
                        '#sample-min' : 'min',
                        '#sample-q1' : 'q1',
                        '#sample-mean' : 'mean',
                        '#sample-median' : 'median',
                        '#sample-q3' : 'q3',
                        '#sample-max' : 'max'
                    };
                    
                    for (var id in idToSummaryValue) {
                        if (!idToSummaryValue.hasOwnProperty(id)) {
                            continue;
                        }
                        
                        $(id).html(benchData.summary[idToSummaryValue[id]]);
                    }
                    
                    // Set bootstrapped statistics
                    $('#lb-title').prop('title', benchData['confidence_level']);
                    $('#ub-title').prop('title', benchData['confidence_level']);
                    
                    var stats = ['mean', 'median', 'sd', 'mad', 'lls', 'r2'];
                    for (var i = 0; i < stats.length; ++i) {
                        var stat = stats[i];
                        $('#' + stat + '-lb').html(benchData[stat].lowerBound);
                        $('#' + stat + '-estimate').html(benchData[stat].estimate);
                        $('#' + stat + '-up').html(benchData[stat].upperBound);
                    }
                    
                    // Set chart data
                    function setSeries(series, data) {
                        series.setData(data.slice(0), false, false, false);
                    }
                    
                    setSeries(kdeChart.get('pdf'), benchData.kde.data);
                    setSeries(kdeChart.get('mean'), benchData.kde.meanData);
                    setSeries(kdeChart.get('median'), benchData.kde.medianData);
                    kdeChart.redraw(false);
                    
                    setSeries(samplesChart.get('sample'), benchData.samples.data);
                    setSeries(samplesChart.get('highSevere'), benchData.samples.highSevereData);
         )***^***",
R"***^***(           setSeries(samplesChart.get('highMild'), benchData.samples.highMildData);
                    setSeries(samplesChart.get('lowMild'), benchData.samples.lowMildData);
                    setSeries(samplesChart.get('lowSevere'), benchData.samples.lowSevereData);
                    samplesChart.redraw(false);
                    
                    setSeries(rawMeasurementsChart.get('measurement'), benchData.rawMeasurements.data);
                    setSeries(rawMeasurementsChart.get('regression'), benchData.rawMeasurements.regression);
                    rawMeasurementsChart.redraw(false);
                    
                    kdeChart.tooltip.options.formatter = function() {
                        return 'Time: <strong>' + this.x + ' ' + 
                                benchData.kde.units + '</strong><br />Density: <strong>' + this.y + '</strong>';
                    };
                    
                    kdeChart.xAxis[0].update({
                        title:{
                            text: 'Time (' + benchData.kde.units + ')'
                        }
                    });
                    
                    samplesChart.tooltip.options.formatter = function() {
                        if (this.series.options.id == 'sample') {
                            return 'Sample: <strong>' + this.x + '</strong><br />Time: <strong>' 
                                + this.y + ' ' + benchData.samples.units + '</strong>';
                        }
                       
                       return this.series.name + ': <strong>' + this.y + ' ' + benchData.samples.units + '</strong>';
                    };
                    
                    samplesChart.yAxis[0].update({
                        title:{
                            text: 'Time (' + benchData.samples.units + ')'
                        }
                    });
                    
                    rawMeasurementsChart.yAxis[0].update({
                        title:{
                            text: 'Time (' + benchData.rawMeasurements.units + ')'
                        }
                    });
                    
                    rawMeasurementsChart.tooltip.options.formatter = function() {
                        return 'Iterations: <strong>' + this.x + ' ' + '</strong><br />Time: <strong>' + 
                            this.y + ' ' + benchData.rawMeasurements.units + '</strong>';
                    };
                }
            
                for (var b in benchmarkData) {
                    $('<li/>', {
                        html: $('<a/>', {id: b, href: '#', text: benchmarkData[b].name}) 
                    }).appendTo('#benchmarks');
                }  
                
                $('#benchmarks a').click(function(e) {
                    e.preventDefault();
                    var target = $(e.target);
                    
                    updateData(target.attr('id'));
                });
                            
                $('#clock-name').text(clockInfo.name);
                $('#steadiness').text(clockInfo.steadiness);   
                updateData('benchmark_1');
            });
        </script>
        
        <style type="text/css">
            body {
                min-width: 800px;
                margin:0;
                padding:0;
                font-family: "Helvetica Neue", Helvetica, Arial, sans-serif;
                background-color: #fff;
            }

            #content {
                padding: 15px;
            }

            nav {
                width:200px;
                float:left;
                background-color: #fff;
                border-radius: 10px;
                border:1px solid #ddd;
                margin-bottom:15px;
            }

            #benchmarks {
                list-style-type: none;
                padding: 0;
                margin:0;
            }

            #benchmarks a{
                display:block;
                padding: 10px 20px;
                text-decoration: none;
                font-size:14px;
                border-top: 1px solid #ddd;
                box-shadow: rgba(255, 255, 255, 0.2) 0px 0px 1px 0px inset;
                color: #777879;
                word-wrap: break-word;
            }

            #benchmarks li:first-child a {
                border-top: none;
                border-top-left-radius: 10px;
                border-top-right-radius: 10px;
            }

            #benchmarks li:last-child a {
                border-bottom-left-radius: 10px;
                border-bottom-right-radius: 10px;
            }

            #benchmarks a:hover{
                background-color: #f5f5f5;
            }

            #benchmarks .current a {
                background-color: #333 ;
                color: #fff;
            }

            main {
                margin-left: 215px;
            }

            h1 {
                color: #222;
                margin:15px 0;
            }

            caption {
                font-weight:bold;
            }

            #sample-summary caption {
                padding-bottom: 10px;
            }

            #sample-summary tr td {
                border-top: 1px solid #D0CDCD;
            }

            #sample-summary td:nth-child(2) {
                color: #111;
                background-color: #F2F2F2;
            }

            #sample-summary, #analyzed-stats {
                border-collapse: collapse;
                float:left
            }

            #analyzed-stats {
                margin:0 0 0 50px;
            }

            #analyzed-stats thead th,
            #analyzed-stats tr td,
            #sample-summary td {
                padding: 10px 15px;
            }

            #analyzed-stats thead th{
                color: #333;
                font-weight:normal;
            }

            #analyzed-stats tr td {
                border-top: 1px solid #D0CDCD;
                color: #777;
            }

            #analyzed-stats td:first-child, #sample-summary td:first-child {
                color: #333;
            }

            #analyzed-stats td:nth-child(3) {
                color: #111;
                background-color: #F2F2F2;
            }

            #separator {
                clear: both;
                padding-top: 15px;
            }

            #kde, #samples, #raw-measurements {
                min-width: 600px;
                margin-bottom:15px;
                border:1px solid #eee;
                box-shadow: 0 0 3px rgba(0, 0, 0, 0.05);
            }

            #kde {
                height:600px;
            }

            #samples, #raw-measurements {
                height: 800px;
            }
            
            #info {
                border-top: 1px solid #E4E4E4;
                background-color: #F2F2F2;
                padding: 10px;
            }
            
            #info dt {
                font-weight:bold;
            }
            
            #info dd {
                margin-bottom:5px;
            }
            
            #clock-info {
                color: #9A9FA4;
                margin-bottom: 0;
            }
        </style>
        
    </head>
    <body>
        <div id="content">
            <nav>
                <ul id="benchmarks">
                </ul>
            </nav>
            
            <main>
                <h1 id="benchmark-name"> Benchmark name </h1>
            
                <table id="sample-summary">
                    <caption> Sample Summary</caption>
	                <tbody>
		                <tr>
			                <td>min</td>
			                <td id="sample-min"></td>
		                </tr>
		                <tr>
			                <td>Q1</td>
			                <td id="sample-q1"></td>
		                </tr>
		                <tr>
			                <td>mean</td>
			                <td id="sample-mean"></td>
		                </tr>
		                <tr>
			                <td>median</td>
			                <td id="sample-median"></td>
		                </tr>		                
		                <tr>
			                <td>Q3</td>
			                <td id="sample-q3"></td>
		                </tr>
		                <tr>
			                <td>max</td>
			                <td id="sample-max"></td>
		                </tr>
	                </tbody>
                </table>
                            
                <table id="analyzed-stats">
                  <caption>Bootstrapped Statistics</caption>
	                <thead>
	                  <th></th>
	                  <th id="lb-title">lower bound</th>
		                <th>sample estimate</th>		                
		                <th id="ub-title">upper bound</th>
	                </thead>
	                <tbody>
		                <tr>
			                <td>mean</td>
			                <td id="mean-lb"></td>
			                <td id="mean-estimate"></td>
			                <td id="mean-up"></td>
		                </tr>
		                <tr>
			                <td>median</td>
			                <td id="median-lb"></td>
			                <td id="median-estimate"></td>
			                <td id="median-up"></td>
		                </tr>
		                <tr>
			                <td>SD</td>
			                <td id="sd-lb"></td>
			                <td id="sd-estimate"></td>
			                <td id="sd-up"></td>
		                </tr>
		                <tr>
			                <td>MAD</td>
			                <td id="mad-lb"></td>
			                <td id="mad-estimate"></td>
			                <td id="mad-up"></td>
		                </tr>
		                <tr>
			                <td>LLS</td>
			                <td id="lls-lb"></td>
			                <td id="lls-estimate"></td>
			                <td id="lls-up"></td>
		                </tr>
		                <tr>
			                <td>r&sup2;</td>
			                <td id="r2-lb"></td>
			       )***^***",
R"***^***(         <td id="r2-estimate"></td>
			                <td id="r2-up"></td>
		                </tr>
	                </tbody>
                </table>

                <div id="separator"></div>
                
                <div id="kde"></div>

                <div id="samples"></div>
                
                <div id="raw-measurements"></div>
            </main>
        </div>
        <div id="info">
            <h3>Statistics</h3>          
            <dl>
                <dt>MAD (Median Absolute Deviation)</dt>
                <dd>
                    The interval [median - MAD, median + MAD] contains half of the measured values.  Unlike the standard deviation the MAD is resilient to outliers.
                </dd>
                <dt>LLS (Least Linear Squares)</dt>
                <dd>
                    An estimate of the time taken to run a single iteration of the benchmarked function which is calculated using  least linear squares regression (through the origin).  This value should be more accurate then some other statistics, such as mean, since it eliminates constant factors (such as measurement overhead). 
                </dd>
                <dt>r&sup2;</dt>
                <dd>
                    A value in the interval [0, 1] which measures the accuracy of the calculated linear regression.  Any value below .99 indicates that the measurements may be suspect (perhaps other processes on the machine are influencing the benchmarks).
                </dd>
                <dt>lower/upper bound</dt>
                <dd>
                    Confidence intervals calculated using bootstrapping which help determine the accuracy of an estimate.  If the configured confidence_level is .95 (the default) then 95% of the estimates calculated when resampling the data were between the lower and upper bounds.  Lower and upper bounds will be close to the estimated value for high quality estimates.  You can hover over the "lower bound" or "upper bound" column titles to see the confidence level which was used.   
                </dd>        
             </dl>
             <h3>Charts</h3>
             <dl>
                <dt>Kernel Density Estimate</dt>
                <dd>
                    Shows the probability of a particular measurement occurring. The higher the probability density line the more likely a measurement is to occur.
                </dd>
                <dt>Samples</dt>
                <dd>
                    Plots the per iteration time of each measurement (total time / number of iterations).  If any of the values are classified as outliers according to the IQR criteria the relevant lines are shown as well.
                </dd>
                <dt>Raw Measurements</dt>
                <dd>
                    The raw measurements(number of iterations and duration) which were collected when benchmarking a function.  The regression line is created from the calculated LLS value.  All points should be on or very near the regression line.
                </dd>
             </dl>
             <p>You can hover over the any of the charts to see exact values and select areas to zoom in.</p>
             <p id="clock-info">All times measured with <span id="clock-name"></span> which is <span id="steadiness"></span>.</p>
        </div>
    </body>
</html>
)***^***"
