$(".test").click(function()
{   
    $("#infobox").text("");
    var kier = $(this).attr('name');
    var param = { "kier": kier };
    $.post( "/tryb", param ).done(function( data, status ) { $("#infobox").text(data); });
});

$(".zap").click(function()
{   
    $("#infobox").text("Zapisuję...");
    var param = 
    {
        "czas":       $('[name="czas"]').val(),
        "czasLEDzap": $('[name="czasLEDzap"]').val(),   
        "czasLEDgas": $('[name="czasLEDgas"]').val(),
        "maxjas":     $('[name="maxjas"]').val(),
        "r":          $('[name="r"]').val(),
        "g":          $('[name="g"]').val(),
        "b":          $('[name="b"]').val(),
        "efekt":      $('[name="efekt"]').val(),
    };
    
    $.post( "/set", param ).done(function( data, status ) 
    {
        $("#infobox").text("Zapisano!");
        $.each(data.dane, function(key,val){ $('[name="'+key+'"]').val(val); });	
    });
})

$(function() 
{
    var colorPicker = new iro.ColorPicker('#color-picker-container', 
    {
        width: 280,
        borderWidth: 5,
        borderColor: "#fff",
    });

    function onColorChange(color, changes)
    {
        var rgb = color.rgb;
        $('[name="r"]').val(rgb["r"]);
        $('[name="g"]').val(rgb["g"]);
        $('[name="b"]').val(rgb["b"]);
    }

    colorPicker.on('color:change', onColorChange);

    var param =  {"tryb": 1 };
    $.post( "/get", param ) .done(function( data, status ) 
    {
        $.each(data, function(key,val){ $('[name="'+key+'"]').val(val);	});	
        colorPicker.color.rgb = { r: data["r"], g: data["g"], b: data["b"] };
    });
});