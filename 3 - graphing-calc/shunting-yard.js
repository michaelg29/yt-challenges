let TYPE_OP = 'o';
let TYPE_CONST = 'c';
let TYPE_FUNC = 'f';
let TYPE_ELSE = 'e';
let TYPE_LPAREN = '(';
let TYPE_RPAREN = ')';

function genFunc(eval, type = TYPE_FUNC, prec = 0, left = true) {
	return {
		eval: eval,
		t: type,
		prec: prec,
		left: left
	};
}

function genNode(val, func = true, unary = true) {
	return {
		val: val,
		func: func,
		unary: unary,
		right: null,
		left: null,
		name: ''
	};
}

let constants = {
	pi: Math.PI,
	e: Math.E
};
let constant_names = Object.keys(constants);

let varnames = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
let variables = {};
for (var i = 0, len = varnames.length; i < len; i++) {
	variables[varnames.charAt(i)] = 0;
}

let unary_functions = {
	sin: genFunc((x) => Math.sin(x)),
	cos: genFunc((x) => Math.cos(x)),
	tan: genFunc((x) => Math.tan(x)),
	arcsin: genFunc((x) => Math.asin(x)),
	arccos: genFunc((x) => Math.acos(x)),
	arctan: genFunc((x) => Math.atan(x)),
	csc: genFunc((x) => 1 / Math.sin(x)),
	sec: genFunc((x) => 1 / Math.cos(x)),
	cot: genFunc((x) => 1 / Math.tan(x)),
	arccsc: genFunc((x) => Math.asin(1 / x)),
	arcsec: genFunc((x) => Math.acos(1 / x)),
	arccot: genFunc((x) => Math.atan(1 / x)),
	ln: genFunc((x) => Math.log(x)),
	log: genFunc((x) => Math.log10(x)),
	sqrt: genFunc((x) => Math.sqrt(x)),
	abs: genFunc((x) => Math.abs(x))
};

let binary_functions = {
	'+': genFunc((x, y) => x + y, TYPE_OP, 2),
	'-': genFunc((x, y) => x - y, TYPE_OP, 2),
	'*': genFunc((x, y) => x * y, TYPE_OP, 3),
	'/': genFunc((x, y) => x / y, TYPE_OP, 3),
	'%': genFunc((x, y) => x % y, TYPE_OP, 3),
	'^': genFunc((x, y) => Math.pow(x, y), TYPE_OP, 4, false),
	max: genFunc((x, y) => Math.max(x, y)),
	min: genFunc((x, y) => Math.min(x, y))
};

let functions = Object.keys(unary_functions).concat(Object.keys(binary_functions));
let operators = '+-*/%^';
let left_brackets = '({[';
let right_brackets = ')}]';

function isNumber(c) {
	if (typeof c === 'number') {
		return true;
	}

	return !isNaN(c) || constant_names.includes(c) || c === '.';
}

function isVariable(c) {
	return varnames.includes(c);
}

function getNumVal(c) {
	if (typeof c === 'number') {
		return c;
	} else if (constant_names.includes(c)) {
		return constants[c];
	} else {
		return parseFloat(c);
	}
}

function isFunction(c) {
	return functions.includes(c);
}

function findElement(i, eqn, list) {
	for (var j = 0, len = list.length; j < len; j++) {
		var n = list[j].length;
		if (eqn.substring(i, i + n) === list[j]) {
			return [true, list[j], n];
		}
	}

	return [false, '', 1];
}

function getPrecedence(op) {
	if (Object.keys(binary_functions).includes(op)) {
		return binary_functions[op].prec;
	}

	return 0;
}

function isLeftAssociative(op) {
	if (Object.keys(binary_functions).includes(op)) {
		return binary_functions[op].left;
	}

	return true;
}

function RPN(eqn) {
	let queue = [];
	let stack = [];

	let obj = '';
	let type = '';

	// for each token
	for (var i = 0, eq_len = eqn.length; i < eq_len; i++) {
		let t = eqn[i];

		if (t === ' ' || t === ',') {
			continue;
		}

		// determine what token is
		if (isNumber(t)) {
			type = TYPE_CONST;

			obj = t;
			if (i < eq_len - 1) {
				// get entire number
				while (isNumber(eqn[i + 1])) {
					obj += eqn[i + 1];
					i++;
					if (i >= eq_len - 1) {
						break;
					}
				}
			}
			obj = getNumVal(obj);
		} else {
			let data = findElement(i, eqn, functions);
			let found = data[0];
			obj = data[1];
			let n = data[2];
			if (found) {
				type = operators.includes(obj) ? TYPE_OP : TYPE_FUNC;
			} else {
				data = findElement(i, eqn, constant_names);
				found = data[0];
				obj = data[1];
				n = data[2];
				if (found) {
					type = TYPE_CONST;
				} else if (isVariable(t)) {
					type = TYPE_CONST;
					obj = t;
				} else {
					if (left_brackets.includes(t)) {
						type = TYPE_LPAREN;
					} else if (right_brackets.includes(t)) {
						type = TYPE_RPAREN;
					} else {
						type = TYPE_ELSE;
					}
				}
			}
			i += n - 1;
		}

		// do something with token
		let last_stack = stack[stack.length - 1];
		switch (type) {
			case TYPE_CONST:
				queue.push(obj);
				break;
			case TYPE_FUNC:
				stack.push(obj);
				break;
			case TYPE_OP:
				if (stack.length != 0) {
					while (
						((functions.includes(last_stack) && !operators.includes(last_stack)) ||
							getPrecedence(last_stack) > getPrecedence(obj) ||
							(getPrecedence(last_stack) === getPrecedence(obj)
								&& isLeftAssociative(last_stack))) &&
						!left_brackets.includes(last_stack)
					) {
						queue.push(stack.pop());
						if (stack.length === 0) {
							break;
						}
						last_stack = stack[stack.length - 1];
					}
				}
				stack.push(obj);
				break;
			case TYPE_LPAREN:
				stack.push('(');
				break;
			case TYPE_RPAREN:
				while (last_stack !== '(') {
					queue.push(stack.pop());
					last_stack = stack[stack.length - 1];
				}
				stack.pop();
				break;
			default:
				return null;
		}
	}

	while (stack.length > 0) {
		queue.push(stack.pop());
	}

	return queue;
}

function parse(rpn) {
	let stack = [];

	Array.from(rpn).forEach((t) => {
		let tr = null;
		if (isNumber(t) || isVariable(t)) {
			tr = genNode(t, false);
		} else {
			if (Object.keys(binary_functions).includes(t)) {
				tr = genNode(binary_functions[t], true, false);

				let a = stack.pop();
				let b = stack.pop();

				if (typeof a === 'number') {
					tr.right = genNode(a, false);
				} else {
					tr.right = a;
				}

				if (typeof b === 'number') {
					tr.left = genNode(b, false);
				} else {
					tr.left = b;
				}
			} else if (Object.keys(unary_functions).includes(t)) {
				tr = genNode(unary_functions[t]);

				a = stack.pop();

				if (typeof a === 'number') {
					tr.left = genNode(a, false);
				} else {
					tr.left = a;
				}
			}
		}
		tr.name = t;
		stack.push(tr);
	});

	return stack.pop();
}

function eval(tree) {
	if (tree.func) {
		if (tree.unary) {
			return tree.val.eval(eval(tree.left));
		} else {
			return tree.val.eval(eval(tree.left), eval(tree.right));
		}
	} else {
		if (constant_names.includes(tree.val)) {
			return constants[tree.val];
		} else if (varnames.includes(tree.val)) {
			return variables[tree.val];
		} else {
			return tree.val;
		}
	}
}