(function ($) {

Drupal.behaviors.rdfuiFieldsetSummaries = {
  attach: function (context) {
    function setSummary() {
     $(this).drupalSetSummary(function (context) {
      return Drupal.checkPlain(
        $(':input', context).not('[type=hidden]').map(
          function () {
            return $(this).closest('.form-item').css('display') === 'none' ? null : $(this).val()
          }).toArray().join(' ')
      )
     })
    }
    $('fieldset.rdf-field', context).each(setSummary);
      console.log('hhhh');
    $(document).bind('state:visible', function () {
      Drupal.behaviors.rdfuiFieldsetSummaries.attach($(this).closest('fieldset.rdf-field')[0])
    })
  }
};

})(jQuery);
