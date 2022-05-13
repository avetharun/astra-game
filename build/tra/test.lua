local s = signal.new()  
function test_callback_int(i)   
    print("test_callback_int: " .. i)   
end     
s:setCallback(test_callback_int)    
s:execute(0)