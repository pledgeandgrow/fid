/**
 * Tree-sitter grammar for Fides (.fd), the language `fid` compiles
 * (competitive-roadmap TL10: the grammar Zed needs).
 *
 * Written from docs/reference/reference.md (sections 1-6, 10, 11) and
 * compiler/parser.fd. It is meant for editors (highlighting, outline,
 * brackets, indents), so it accepts a little more than `fid` does and
 * parses the domain declarations (layer, model, component, system, state,
 * on) and the bodies of select/parallel/handle-with as token trees.
 *
 * Generate the parser (src/parser.c, which Zed compiles) with
 *   cd editors/tree-sitter-fides && tree-sitter generate && tree-sitter test
 * tests/tooling/zed-check.sh checks this file without the CLI, and with it
 * parses every .fd file of stdlib/ and tests/bootstrap/.
 */

const PREC = {
  pipe: 1,
  coalesce: 2,
  range: 3,
  or: 4,
  bitor: 5,
  bitxor: 6,
  bitand: 7,
  shift: 8,
  and: 9,
  equal: 10,
  compare: 11,
  add: 12,
  multiply: 13,
  unary: 14,
  postfix: 15,
  call: 16,
  field: 17,
};

const PRIMITIVES = [
  'i8', 'i16', 'i32', 'i64', 'i128',
  'u8', 'u16', 'u32', 'u64', 'u128',
  'f32', 'f64', 'bool', 'str', 'void', 'q8', 'q4',
];

const ASSIGN_OPS = ['=', '+=', '-=', '*=', '/=', '%=', '&=', '|=', '^=', '<<=', '>>='];

// every operator and punctuation token, for token trees
const PUNCT = [
  '+', '-', '*', '/', '%', '=', '==', '!=', '<', '<=', '>', '>=', '<<', '>>',
  '&', '&&', '|', '||', '|>', '^', '~', '~>', '!', '.', '..', '..=', '...',
  ',', ':', '::', ';', '?', '??', '?.', '#', '@', '->', '=>',
  '+=', '-=', '*=', '/=', '%=', '&=', '|=', '^=', '<<=', '>>=',
];

function commaSep(rule) {
  return optional(commaSep1(rule));
}

function commaSep1(rule) {
  return seq(rule, repeat(seq(',', rule)), optional(','));
}

function sepBy1(sep, rule) {
  return seq(rule, repeat(seq(sep, rule)));
}

module.exports = grammar({
  name: 'fides',

  word: $ => $.identifier,

  extras: $ => [/\s/, $.line_comment, $.block_comment],

  conflicts: $ => [
    // `if x {`: `x` and a block, or the struct literal `x { f: v }`
    [$._expression, $.struct_expression],
  ],

  rules: {
    source_file: $ => repeat($._item),

    _item: $ => choice(
      $.module_declaration,
      $.use_declaration,
      $.function_item,
      $.struct_item,
      $.enum_item,
      $.impl_item,
      $.cap_item,
      $.const_item,
      $.global_item,
      $.type_alias,
      $.extern_block,
      $.annotation,
      $.attribute_item,
      $.domain_item,
      ';',
    ),

    // ---- items ----------------------------------------------------------

    module_declaration: $ => prec.right(seq('module', field('name', $.path), optional(';'))),

    path: $ => sepBy1('::', $.identifier),

    use_declaration: $ => prec.right(seq(
      'use',
      field('path', $.use_path),
      optional(seq('as', field('alias', $.identifier))),
      optional($.use_list),
      optional(seq('with', $.capability_list)),
      optional(';'),
    )),

    use_path: $ => seq(
      $.identifier,
      repeat(seq('::', choice($.identifier, '*', $.use_list))),
    ),

    use_list: $ => seq('{', commaSep(choice($.identifier, '*')), '}'),

    capability_list: $ => seq('{', commaSep($.identifier), '}'),

    visibility: _ => 'pub',

    function_item: $ => prec.right(seq(
      optional($.visibility),
      repeat(choice('async', 'comptime', 'interrupt')),
      'fn',
      field('name', $.identifier),
      optional(field('type_parameters', $.type_parameters)),
      field('parameters', $.parameters),
      optional(seq('->', field('return_type', $._type))),
      optional(field('effects', $.effects)),
      repeat($.contract),
      optional(choice(field('body', $.block), ';')),
    )),

    type_parameters: $ => seq('<', commaSep1($.type_parameter), '>'),

    type_parameter: $ => seq(
      optional('const'),
      field('name', $.identifier),
      optional(seq(':', sepBy1('+', $._type))),
    ),

    parameters: $ => seq('(', commaSep($.parameter), ')'),

    parameter: $ => seq(
      repeat($.annotation),
      optional('...'),
      optional('mut'),
      field('name', $.identifier),
      optional(seq(
        ':',
        field('type', $._type),
        optional(seq('=', field('default', $._expression))),
      )),
    ),

    effects: $ => seq('!', choice(
      $.identifier,
      seq('{', commaSep($.identifier), '}'),
    )),

    contract: $ => seq(choice('given', 'ensure'), $._expression, optional(',')),

    struct_item: $ => seq(
      optional($.visibility),
      'struct',
      field('name', alias($.identifier, $.type_identifier)),
      optional(field('type_parameters', $.type_parameters)),
      field('body', $.field_declaration_list),
    ),

    field_declaration_list: $ => seq('{', repeat(seq($.field_declaration, optional(','))), '}'),

    field_declaration: $ => seq(
      optional($.visibility),
      field('name', $.identifier),
      ':',
      field('type', $._type),
      optional(seq('=', field('default', $._expression))),
    ),

    enum_item: $ => seq(
      optional($.visibility),
      'enum',
      field('name', alias($.identifier, $.type_identifier)),
      optional(field('type_parameters', $.type_parameters)),
      field('body', $.enum_variant_list),
    ),

    enum_variant_list: $ => seq('{', repeat(seq($.enum_variant, optional(','))), '}'),

    enum_variant: $ => seq(
      field('name', $.identifier),
      optional(seq('(', commaSep($.variant_field), ')')),
    ),

    variant_field: $ => seq(
      optional(seq(field('name', $.identifier), ':')),
      field('type', $._type),
    ),

    impl_item: $ => seq(
      'impl',
      optional(field('type_parameters', $.type_parameters)),
      field('type', $._type),
      optional(seq('for', field('for', $._type))),
      field('body', $.declaration_list),
    ),

    declaration_list: $ => seq('{', repeat(choice(
      $.function_item,
      $.associated_type,
      $.annotation,
      ';',
    )), '}'),

    associated_type: $ => prec.right(seq(
      'type',
      field('name', alias($.identifier, $.type_identifier)),
      optional($.type_parameters),
      optional(seq('=', field('type', $._type))),
      optional(';'),
    )),

    cap_item: $ => seq(
      optional($.visibility),
      'cap',
      field('name', alias($.identifier, $.type_identifier)),
      optional(field('type_parameters', $.type_parameters)),
      optional(seq(':', sepBy1('+', $._type))),
      field('body', $.declaration_list),
    ),

    const_item: $ => prec.right(seq(
      optional($.visibility),
      'const',
      field('name', $.identifier),
      optional(seq(':', field('type', $._type))),
      '=',
      field('value', $._expression),
      optional(';'),
    )),

    global_item: $ => prec.right(seq(
      optional($.visibility),
      choice('var', 'static', 'val'),
      field('name', $.identifier),
      optional(seq(':', field('type', $._type))),
      optional(seq('=', field('value', $._expression))),
      optional(';'),
    )),

    type_alias: $ => prec.right(seq(
      optional($.visibility),
      'type',
      field('name', alias($.identifier, $.type_identifier)),
      optional($.type_parameters),
      '=',
      field('type', $._type),
      optional(';'),
    )),

    extern_block: $ => seq(
      'extern',
      optional(field('abi', $.string_literal)),
      repeat(seq(choice('include', 'link'), $.string_literal)),
      choice(
        seq('{', repeat($.extern_function), '}'),
        $.extern_function,
      ),
    ),

    extern_function: $ => prec.right(seq(
      optional($.visibility),
      'fn',
      field('name', $.identifier),
      optional(seq('as', field('alias', $.identifier))),
      field('parameters', $.parameters),
      optional(seq('->', field('return_type', $._type))),
      optional(';'),
    )),

    // `@name`, `@no-alloc`, `@gas(100)`, `@dp(epsilon=0.5)`
    annotation: $ => prec.right(seq(
      '@',
      field('name', $.identifier),
      repeat(seq(token.immediate('-'), $.identifier)),
      optional(field('arguments', $.paren_token_tree)),
    )),

    attribute_item: $ => seq('#', $.bracket_token_tree),

    // layer, model, component, system, state and on declarations
    domain_item: $ => seq(
      field('kind', choice('layer', 'model', 'component', 'system', 'state', 'on')),
      field('name', $.identifier),
      optional($.parameters),
      field('body', $.token_tree),
    ),

    // ---- token trees ----------------------------------------------------

    token_tree: $ => seq('{', repeat($._token_tree_item), '}'),
    paren_token_tree: $ => seq('(', repeat($._token_tree_item), ')'),
    bracket_token_tree: $ => seq('[', repeat($._token_tree_item), ']'),

    _token_tree_item: $ => choice(
      $.token_tree,
      $.paren_token_tree,
      $.bracket_token_tree,
      $.identifier,
      $.primitive_type,
      $._literal,
      ...PUNCT,
    ),

    // ---- types ----------------------------------------------------------

    _type: $ => choice(
      $.primitive_type,
      alias($.identifier, $.type_identifier),
      $.generic_type,
      $.scoped_type,
      $.reference_type,
      $.pointer_type,
      $.array_type,
      $.tuple_type,
      $.function_type,
      $.dyn_type,
      $.never_type,
    ),

    primitive_type: _ => choice(...PRIMITIVES),

    generic_type: $ => prec(1, seq(
      field('type', alias($.identifier, $.type_identifier)),
      field('type_arguments', $.type_arguments),
    )),

    type_arguments: $ => seq('<', commaSep1(choice(
      $._type,
      $.integer_literal,
      $.dimension_list,
    )), '>'),

    // the shape of Tensor<f64, [2, 3]>
    dimension_list: $ => seq('[', commaSep(choice($.integer_literal, $.identifier)), ']'),

    scoped_type: $ => prec(1, seq(
      field('path', alias($.identifier, $.type_identifier)),
      '::',
      field('name', alias($.identifier, $.type_identifier)),
      optional($.type_arguments),
    )),

    reference_type: $ => prec(1, seq('&', optional('mut'), field('type', $._type))),

    pointer_type: $ => prec(1, seq('*', optional(choice('mut', 'const')), field('type', $._type))),

    array_type: $ => seq(
      '[',
      field('element', $._type),
      ';',
      field('length', choice($.integer_literal, $.identifier)),
      ']',
    ),

    tuple_type: $ => seq('(', commaSep($._type), ')'),

    function_type: $ => prec.right(seq(
      'fn',
      '(',
      commaSep($._type),
      ')',
      optional(seq('->', field('return_type', $._type))),
    )),

    dyn_type: $ => prec(1, seq('dyn', field('trait', $._type))),

    never_type: _ => '!',

    // ---- statements -----------------------------------------------------

    block: $ => seq('{', repeat($._statement), '}'),

    _statement: $ => choice(
      $.let_declaration,
      $.assignment_statement,
      $.expression_statement,
      $.return_statement,
      $.break_statement,
      $.continue_statement,
      $.while_statement,
      $.for_statement,
      $.loop_statement,
      $.guard_statement,
      $.defer_statement,
      $.region_statement,
      $.query_statement,
      $.grad_statement,
      $.select_statement,
      $.parallel_statement,
      $.handle_statement,
      $.annotation,
      ';',
    ),

    let_declaration: $ => prec.right(seq(
      optional('persist'),
      choice('val', 'var'),
      field('pattern', choice(
        $.identifier,
        seq('(', commaSep($.identifier), ')'),
      )),
      optional(seq(':', field('type', $._type))),
      optional(seq('=', field('value', $._expression))),
      optional(';'),
    )),

    assignment_statement: $ => prec.right(seq(
      field('left', $._expression),
      field('operator', choice(...ASSIGN_OPS)),
      field('right', $._expression),
      optional(';'),
    )),

    // the `;` is optional: a statement ends where its expression ends
    expression_statement: $ => choice(
      seq($._expression, ';'),
      prec(-1, $._expression),
    ),

    return_statement: $ => prec.right(seq('return', optional($._expression), optional(';'))),

    break_statement: $ => prec.right(seq('break', optional($._expression), optional(';'))),

    continue_statement: $ => prec.right(seq('continue', optional(';'))),

    while_statement: $ => seq(
      'while',
      field('condition', $._expression),
      field('body', $.block),
    ),

    for_statement: $ => seq(
      'for',
      field('pattern', $.identifier),
      'in',
      field('value', $._expression),
      field('body', $.block),
    ),

    loop_statement: $ => seq('loop', field('body', $.block)),

    guard_statement: $ => seq(
      'guard',
      field('condition', $._expression),
      '=>',
      field('body', $._statement),
    ),

    defer_statement: $ => seq('defer', field('body', $.block)),

    // region r { }, region r with budget(64) { }, region gpu r { }
    region_statement: $ => seq(
      'region',
      optional(choice('gpu', 'kv_cache')),
      field('name', $.identifier),
      optional(seq('with', repeat1($.region_option))),
      field('body', $.block),
    ),

    // budget(64), window(8)
    region_option: $ => seq(field('name', $.identifier), $.arguments),

    query_statement: $ => seq(
      'query',
      '(',
      commaSep(alias($.identifier, $.type_identifier)),
      ')',
      field('body', $.block),
    ),

    grad_statement: $ => seq('grad', $.arguments, field('body', $.block)),

    // unsafe { }, sandbox { }, txn { }, spawn scope { }
    special_block: $ => seq(
      choice('unsafe', 'sandbox', 'txn', seq('spawn', 'scope')),
      field('body', $.block),
    ),

    select_statement: $ => seq('select', field('body', $.token_tree)),

    parallel_statement: $ => seq('parallel', field('body', $.token_tree)),

    handle_statement: $ => seq(
      'handle',
      field('body', $.block),
      'with',
      field('handlers', $.token_tree),
    ),

    // ---- expressions ----------------------------------------------------

    _expression: $ => choice(
      $.binary_expression,
      $.unary_expression,
      $.reference_expression,
      $.prefix_expression,
      $.cast_expression,
      $.is_expression,
      $.try_expression,
      $.call_expression,
      $.field_expression,
      $.optional_field_expression,
      $.index_expression,
      $.range_expression,
      $.identifier,
      $.scoped_identifier,
      $.generic_function,
      $._literal,
      $.array_expression,
      $.tuple_expression,
      $.unit_expression,
      $.parenthesized_expression,
      $.block,
      $.if_expression,
      $.match_expression,
      $.closure_expression,
      $.struct_expression,
      $.comptime_expression,
      $.try_block,
      $.special_block,
      $.resume_expression,
      $.backward_expression,
      $.macro_invocation,
    ),

    binary_expression: $ => {
      const table = [
        [PREC.pipe, '|>'],
        [PREC.coalesce, '??'],
        [PREC.or, '||'],
        [PREC.bitor, '|'],
        [PREC.bitxor, '^'],
        [PREC.bitand, '&'],
        [PREC.shift, choice('<<', '>>')],
        [PREC.and, '&&'],
        [PREC.equal, choice('==', '!=')],
        [PREC.compare, choice('<', '<=', '>', '>=')],
        [PREC.add, choice('+', '-')],
        [PREC.multiply, choice('*', '/', '%')],
      ];
      return choice(...table.map(([p, op]) => prec.left(p, seq(
        field('left', $._expression),
        field('operator', op),
        field('right', $._expression),
      ))));
    },

    range_expression: $ => prec.left(PREC.range, seq(
      field('start', $._expression),
      choice('..', '..='),
      field('end', $._expression),
    )),

    unary_expression: $ => prec(PREC.unary, seq(
      field('operator', choice('-', '!', '~', '*')),
      field('operand', $._expression),
    )),

    reference_expression: $ => prec(PREC.unary, seq(
      '&',
      optional('mut'),
      field('value', $._expression),
    )),

    // await e, spawn e, yield e, do E.op(x)
    prefix_expression: $ => prec(PREC.unary, seq(
      field('operator', choice('await', 'spawn', 'yield', 'do')),
      field('operand', $._expression),
    )),

    cast_expression: $ => prec.left(PREC.postfix, seq(
      field('value', $._expression),
      choice('as', '~>'),
      field('type', $._type),
    )),

    is_expression: $ => prec.left(PREC.compare, seq(
      field('value', $._expression),
      'is',
      field('type', $._type),
    )),

    try_expression: $ => prec(PREC.postfix, seq($._expression, '?')),

    call_expression: $ => prec(PREC.call, seq(
      field('function', $._expression),
      field('arguments', $.arguments),
    )),

    arguments: $ => seq('(', commaSep(choice($._expression, $.named_argument)), ')'),

    named_argument: $ => seq(field('name', $.identifier), ':', field('value', $._expression)),

    field_expression: $ => prec(PREC.field, seq(
      field('value', $._expression),
      '.',
      field('field', choice($.identifier, $.integer_literal)),
    )),

    optional_field_expression: $ => prec(PREC.field, seq(
      field('value', $._expression),
      '?.',
      field('field', $.identifier),
    )),

    index_expression: $ => prec(PREC.postfix, seq(
      field('value', $._expression),
      '[',
      field('index', $._expression),
      ']',
    )),

    // Vec::new, Option::Some, m::f
    scoped_identifier: $ => seq(
      field('path', choice($.identifier, $.scoped_identifier, $.generic_function)),
      '::',
      field('name', $.identifier),
    ),

    // f::<i64>, Vec::<str>
    generic_function: $ => seq(
      field('function', choice($.identifier, $.scoped_identifier)),
      '::',
      field('type_arguments', $.type_arguments),
    ),

    array_expression: $ => seq('[', commaSep($._expression), ']'),

    tuple_expression: $ => seq(
      '(',
      $._expression,
      ',',
      commaSep($._expression),
      ')',
    ),

    unit_expression: _ => seq('(', ')'),

    parenthesized_expression: $ => seq('(', $._expression, ')'),

    if_expression: $ => prec.right(seq(
      'if',
      field('condition', $._expression),
      field('consequence', $.block),
      optional(seq('else', field('alternative', choice($.block, $.if_expression)))),
    )),

    match_expression: $ => seq(
      'match',
      field('value', $._expression),
      field('body', $.match_block),
    ),

    match_block: $ => seq('{', repeat($.match_arm), '}'),

    match_arm: $ => prec.right(seq(
      field('pattern', $._pattern),
      optional(seq('if', field('guard', $._expression))),
      '=>',
      field('value', $._expression),
      optional(','),
    )),

    closure_expression: $ => prec.right(seq(
      'fn',
      field('parameters', $.parameters),
      optional(seq('->', field('return_type', $._type))),
      choice(field('body', $.block), seq('=>', field('body', $._expression))),
    )),

    struct_expression: $ => prec.dynamic(-1, seq(
      field('name', choice($.identifier, $.scoped_identifier, $.generic_function)),
      field('body', $.field_initializer_list),
    )),

    field_initializer_list: $ => seq('{', commaSep(choice(
      $.field_initializer,
      $.base_field_initializer,
    )), '}'),

    field_initializer: $ => seq(field('field', $.identifier), ':', field('value', $._expression)),

    // ..base, or a bare `..` (every other field takes its default)
    base_field_initializer: $ => prec.right(seq('..', optional($._expression))),

    comptime_expression: $ => prec(PREC.unary, seq('comptime', choice(
      seq('gen', $.block),
      $._expression,
    ))),

    try_block: $ => seq('try', $.block),

    resume_expression: $ => seq('resume', $.arguments),

    backward_expression: $ => seq('backward', $.arguments),

    // asm!("...", in("rax") x, out("rbx") y)
    macro_invocation: $ => seq(
      field('macro', $.identifier),
      token.immediate('!'),
      $.paren_token_tree,
    ),

    // ---- patterns -------------------------------------------------------

    _pattern: $ => choice(
      $.wildcard_pattern,
      $.integer_literal,
      $.char_literal,
      $.range_pattern,
      $.string_literal,
      $.boolean_literal,
      $.identifier,
      $.scoped_identifier,
      $.tuple_struct_pattern,
    ),

    wildcard_pattern: _ => '_',

    range_pattern: $ => seq(
      choice($.integer_literal, $.char_literal),
      choice('..=', '..'),
      choice($.integer_literal, $.char_literal),
    ),

    tuple_struct_pattern: $ => seq(
      field('type', choice($.identifier, $.scoped_identifier)),
      '(',
      commaSep($.binding),
      ')',
    ),

    binding: $ => seq(
      field('name', choice($.identifier, $.wildcard_pattern)),
      optional(seq(':', field('type', $._type))),
    ),

    // ---- literals -------------------------------------------------------

    _literal: $ => choice(
      $.integer_literal,
      $.float_literal,
      $.char_literal,
      $.string_literal,
      $.raw_string_literal,
      $.format_string,
      $.boolean_literal,
    ),

    integer_literal: _ => token(choice(
      /[0-9][0-9_]*/,
      /0[xX][0-9a-fA-F]+/,
      /0[bB][01]+/,
      /0[oO][0-7]+/,
    )),

    float_literal: _ => token(/[0-9][0-9_]*\.[0-9][0-9_]*([eE][+-]?[0-9]+)?/),

    char_literal: _ => token(seq(
      '\'',
      choice(
        /[^'\\\n]/,
        /\\[ntr0\\'"]/,
        /\\x[0-9a-fA-F]{2}/,
        /\\u\{[0-9a-fA-F]{1,6}\}/,
      ),
      '\'',
    )),

    string_literal: $ => seq(
      '"',
      repeat(choice($.escape_sequence, $._string_content)),
      token.immediate('"'),
    ),

    _string_content: _ => token.immediate(prec(1, /[^"\\]+/)),

    escape_sequence: _ => token.immediate(seq('\\', choice(
      /[ntr0\\'"]/,
      /x[0-9a-fA-F]{2}/,
      /u\{[0-9a-fA-F]{1,6}\}/,
      /./,
    ))),

    // r"..." and r#"..."# (one `#`)
    raw_string_literal: _ => token(choice(
      seq('r"', /[^"]*/, '"'),
      seq('r#"', /([^"]|"[^#])*/, '"#'),
    )),

    // f"x = {x}" (no escape for a brace)
    format_string: $ => seq(
      'f"',
      repeat(choice($.escape_sequence, $._format_content, $.interpolation)),
      token.immediate('"'),
    ),

    _format_content: _ => token.immediate(prec(1, /[^"\\{]+/)),

    interpolation: $ => seq(token.immediate('{'), $._expression, '}'),

    boolean_literal: _ => choice('true', 'false'),

    identifier: _ => /[A-Za-z_][A-Za-z0-9_]*/,

    line_comment: _ => token(seq('//', /[^\n]*/)),

    block_comment: _ => token(seq('/*', /[^*]*\*+([^/*][^*]*\*+)*/, '/')),
  },
});
