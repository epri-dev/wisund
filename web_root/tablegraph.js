function AutoTable(selector, withcolor) {
  var tBody = d3.select(selector).selectAll("tbody");
  var withcolor = withcolor;

  this.update = function(newdata) {
      var tr = tBody.selectAll("tr")
          .data(newdata, function(d){return d[0];});
      if (withcolor) 
        tr.style("color", function(d,i,j){ return "hsl("+i*360/newdata.length+",100%,50%)";})

      tr.enter().append("tr")
          .style("background-color", "white");

      tr.order();

      var td = tr.selectAll("td")
          .data(function(d) {
              var retval = [];
              for (var i=0; i<d.length; ++i) {
                  retval.push([i, d[i]]);
              }
              return retval;
          });

      td.enter().append("td")
              .on("mouseover", function(){d3.select(this).style("background-color", "aliceblue")}) 
              .on("mouseout", function(){d3.select(this).style("background-color", "white")});

      td.text(function(d){return d[1];});

      td.exit().remove();

      tr.exit().remove(); 
  }
}

function Linegraph(mytag, mywidth, myheight, pointcount, duration, numlines) {
  var n = pointcount,
      duration = duration,
      now = new Date(Date.now() - duration),
      data = new Array(numlines);

  for (var i=0; i < numlines; ++i) {
      data[i] = new Array(n).fill(0);
  }
  var margin = {top: 6, right: 0, bottom: 20, left: 40},
      width = mywidth - margin.right,
      height = myheight - margin.top - margin.bottom;

  var x = d3.scaleTime()
      .domain([now - (n - 2) * duration, now - duration])
      .range([0, width]);

  var y = d3.scaleLinear()
      .range([height, 0]);

  var yAxis = d3.axisLeft(y)
      .scale(y)
      .ticks(5);

  var line = d3.line()
      .x(function(d, i) { return x(now - (n - 1 - i) * duration); })
      .y(function(d, i) { return y(d); });

  var svg = d3.select(mytag).append("p").append("svg")
      .attr("width", width + margin.left + margin.right)
      .attr("height", height + margin.top + margin.bottom)
    .append("g")
      .attr("transform", "translate(" + margin.left + "," + margin.top + ")");

  var vertaxis = svg.append("g")
      .attr("class", "y axis")
      .call(y.axis = yAxis);

  svg.append("defs").append("clipPath")
      .attr("id", "clip")
    .append("rect")
      .attr("width", width)
      .attr("height", height);

  var axis = svg.append("g")
      .attr("class", "x axis")
      .attr("transform", "translate(0," + height + ")")
      .call(x.axis = d3.axisBottom(x));

  var path=[];
  for (var i=0; i < numlines; ++i) {
      path.push(svg.append("g")
          .attr("clip-path", "url(#clip)")
        .append("path")
          .datum(data[i])
          .attr("class", "line")
          .attr("id", "line"+i)
          .attr("fill", "none")
          .attr("style", "stroke:hsl("+ i*360/numlines +",100%,50%);"));
  }

  var transition = d3.select({})
      .transition()
      .duration(duration)
      .ease(d3.easeLinear);

  this.update = function(newdata) {
      if (newdata.length != data.length) {
          return;
      }
      // update the domains
      now = new Date();
      x.domain([now - (n - 2) * duration, now - duration]);
      ymax = d3.max(newdata);
      y.domain([0, ymax > 1 ? ymax : 1]);

      // push the values on the stacks
      for (var i=0; i < newdata.length; ++i) {
          data[i].push(newdata[i]);
      }

      // redraw the lines
      for (var i=0; i < newdata.length; ++i) {
          svg.select("#line"+i)
              .attr("d", line)
              .attr("transform", null);
      }

      // slide the x-axis left
      axis.call(x.axis);
      // update the y-axis scale
      vertaxis.call(y.axis);

      for (var i=0; i < newdata.length; ++i) {
          // slide the line left
          path[i].transition()
              .attr("transform", "translate(" + x(now - (n - 1) * duration) + ")");
      }

      // pop the old data point off the front
      for (var i=0; i < newdata.length; ++i) {
          data[i].shift();
      }
  };
}
