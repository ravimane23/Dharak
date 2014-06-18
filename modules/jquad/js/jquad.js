// Create jquad, jQuery UI Account Dialogs namespace
var jquad = jquad || {};
(function($, $$) {
  // Extend jquad's namespace with methods
  $$ = $.extend(true, {}, {
    _dialog: null,
    init: function(){
      $$.forms = Drupal.settings.jquad.forms;
      $$.paths = Drupal.settings.jquad.paths;
      $$.settings = Drupal.settings.jquad.settings;
      // delete Drupal.settings.jquad;
    },
    dialog: function(options){
      // Defaults
      var options = options || {};
      options = $.extend({}, {
        title: 'User Account',
        // bgiframe: true,
  			draggable: false,
  			resizable: false,
  			width: 400,
  			modal: true,
  			form: false,
  			close: function(){
  			  $(this).dialog('destroy');
  			  $$._dialog.remove();
  			},
  			buttons: {
  			  'Close': function() {
  					$(this).dialog('destroy');				
  				}
  			}		
      }, options);
      
      // Processing AJAX throbber
      $$.throbber = $('<img/>').css({
		    display: 'none',
		    float: 'left'
		  }).attr('src', $$.paths.throbber.absolute);
		  options = $.extend({}, {
  		  open: function(e, ui) {
  			  $$._dialog.parent().find('.ui-dialog-buttonpane').prepend($$.throbber);
  		  }
  		}, options);
          
      // Establish dialog DOM
      $$._dialog = $('#jquad');
      if ($$._dialog.length) {
        $$._dialog.dialog('destroy');
        $$._dialog.remove();
      }
      $$._dialog = $('<div/>').attr('id', 'jquad');
      if (options.html) {
        $$._dialog.html(options.html);
      }
      if (options.form) {
        if (typeof options.form == 'string') {
          $$.form = $(options.form);
        }
        $$._dialog.html();
        $$._dialog.append(options.form);
        delete options.form;
      }
      $$._dialog.dialog(options)
    },
    centerDialog: function(){
      if ($$._dialog.length) {
        $$._dialog.dialog('option', { position: 'center' });
      }
    },
    formError: function(messages){
      $$.formErrors = $$._dialog.find('div.jquad-messages');
      if (!$$.formErrors.length) {
        $$.formErrors = $('<div/>').addClass('jquad-messages ui-state-error ui-corner-all');
        $$._dialog.prepend($$.formErrors);
      }
      var ul = $('<ul/>');
      $.each(messages, function(key, message){
        var element = $('input[name="' + key + '"]', $$.form);
        if (element.length) {
          element.addClass('error');
        }
        var icon = $('<span/>').addClass('ui-icon ui-icon-alert');
        var li = $('<li/>').html(message);
        li.prepend(icon);
        ul.append(li);
      });
      $$.formErrors.html(ul);
      $$.centerDialog();
    },
    resetForm: function(){
      $$.formErrors = $$._dialog.find('div.jquad-messages');
      if ($$.formErrors.length) {
        $$.formErrors.remove();
        $$.centerDialog();
      }
      $('input', $$.form).each(function(){
        $(this).removeClass('error');
      });
    },
    getForm: function(formId){
      var returnForm = false;
      $.ajax({
        async: false,
        url: $$.paths.getForm.relative + '/' + formId,
        data: {
          destination: $$.paths.current.path
        },
        dataType: 'json',
        type: 'POST',
        error: function(xhr, status) {
          returnForm = false;
        },
        success: function(json) {
          returnForm = json;
        }
      });
      return returnForm;
    },
    bindForm: function(form) {
      var form = form || $$.form;
      // Process Drupal behaviors
      Drupal.attachBehaviors(form);
      // Bind hidden elements as jQuery data
      $('input[type="hidden"]', form).each(function(){
        element = $(this);
        form.data(element.attr('name'), element.val());
      });
      $('input.form-text', form).keypress(function(e){
        if (e.which == 13) {
          $$.form.submit();
        }
      });
      // Capture the form buttons
      form.data('buttons', $('input[type="submit"]', form));
      // Bind form submit event handler
      form.submit(function(){
        $$.processForm($(this));
        return false;
      });      
    },
    processForm: function(form) {
      var form = form || $$.form;
      var postData = {};
      $.map(form.serializeArray(), function(n, i){
        postData[n['name']] = n['value'];
      });
      // Proccess button option
      var op = $$.form.data('op');
      if (op) {
        postData.op = op;
      }
      if ($.md5 !== undefined && typeof $.md5 === 'function' && $$.settings.md5 && postData.pass) {
        postData.md5 = 1;
        postData.pass = $.md5(postData.pass);
      }
      var windowname = false;
      if ($$.paths.current.secure != $$.paths[form.data('form_id')].secure) {
        windowname = true;
        postData.windowname = true;
      }
      $$.resetForm();
      $$.throbber.fadeIn();
      $.ajax({
        url: $$.paths[form.data('form_id')].absolute + '/process',
        'windowname': windowname,
        'data': postData,
        cache: false,
        type: 'POST',
        dataType: 'json',
        error: function(xhr, textStatus) {
          $$.throbber.fadeOut();
          var formId = form.data('form_id');
          var destination = '?destination=' + $$.paths.current.path;
          $$._dialog.html('An error has occured while attempting to login.');
          $$._dialog.dialog('option', { position: 'center', buttons: {
      				'Continue': function() {
      				  location.href = $$.paths[formId].absolute + destination;
        			}
    			  }
    			});
        },
        success: function(json){
          $$.throbber.fadeOut();
          if (json.status === true) {
            if (json.redirect) {
              location.href = json.redirect;
            }
            else if (json.callback && $$[json.callback]) {
              if (json.callbackArg) {
                $$[json.callback](json.callbackArg);
              }
              else {
                $$[json.callback]();
              }
            }
            else {
              $$.refreshPage();
            }
          }
          else {
            $$.formError(json.messages)
          }
        }
      });
    },
    refreshPage: function(){
      location.reload();
    },
    bindLinks: function(){
      // Bind allowed form links to dialogs
      $.each($$.forms, function(formId, menuPath){
        $("a[href^='/" + menuPath + "'], a[href^='/?q=" + menuPath + "']").each(function(){
          $(this).click(function(){
            var link = $(this);
            var dialogData = {
              buttons: {
        				'Close': function() {
        					$(this).dialog('destroy');				
        				}
              }
            };
            // Create dialog title from link title or text
            if (link.attr('title') != '') {
              dialogData.title = link.attr('title');
        	  }
        	  else {
              dialogData.title = link.text();
            }
            // Get form from server
            var json = $$.getForm(formId);
            if (json) {
              if (json.status) {
                if (json.form) {
                  $$.form = $(json.form);
                  $$.bindForm($$.form);
                  var buttons = $$.form.data('buttons');
                  if (buttons.length) {
                    dialogData.buttons = {};
                    buttons.each(function(){
                      dialogData.buttons[$(this).val()] = function(e) {
                        $$.form.data('op', $(e.currentTarget).text())
              					$$.form.submit();
                		  }
                    });
                    dialogData.buttons['Cancel'] = function() {
              					$(this).dialog('destroy');				
              			}
              			$(buttons, $$.form).hide();
              		}
                  dialogData.form = $$.form;
                }
                else if (json.html) {
                  dialogData.html = json.html;
                }
                else {
                  dialogData.html = Drupal.t('An error occurred while retrieving the form.');
                }
              }
            }
            else {
              dialogData.html = Drupal.t('An error occurred while retrieving the form.');
            }
            $$.dialog(dialogData);
            return false;
          });
        });
      });    
    }
  }, $$);
  
  Drupal.behaviors.jquad = function(context) {
    $$.init();
    $$.bindLinks();
  }
})(jQuery, jquad);
