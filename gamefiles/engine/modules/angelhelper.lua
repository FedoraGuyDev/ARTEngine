function sum(a,b) return a + b end
function sub(a,b) return a - b end
function mul(a,b) return a * b end
function div(a,b) return a / b end

function switch(val,cases)
    local fn = cases[val] or cases["default"]
    if fn then return fn() end
end

function array(...)
    local t = {...}
    return setmetatable(t,{
        __index = function(t,k) return rawget(t,k+1) end,
        __newindex = function(t,k,v) rawset(t,k+1,v) end
    })
end

