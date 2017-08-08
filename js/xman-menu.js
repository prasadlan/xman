$(document).ready(function() {
    var $ccItems = $('#control_center_menu span');
    if ($ccItems.length !== 0) {
        $ccItems.last().append($('#xman-menu-item-wrapper').html());
    }
    else {
        var $projMenu = $('#projMenuApplications');
        if ($projMenu.length !== 0) {
            $projMenu.parent().siblings('.x-panel-bwrap').find('.menubox .menubox').append('<div class="hang">' + $('#xman-menu-item-wrapper').html() + '</div>');
        }
    }
});
